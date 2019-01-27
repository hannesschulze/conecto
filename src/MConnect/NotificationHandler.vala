/**
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * AUTHORS
 * Maciek Borzecki <maciek.borzecki (at] gmail.com>
 * gyan000 <gyan000 (at] ijaz.fr>
 */
using Gee;
using Conecto;

namespace MConnect {

    class NotificationHandler : Object, PacketHandlerInterface {

        public signal void notification_received (Notification notification);

        public const string NOTIFICATION = "kdeconnect.notification";
        public const string NOTIFICATION_REQUEST = "kdeconnect.notification.request";
        public const string NOTIFICATION_REPLY = "kdeconnect.notification.reply";
        // Not from KDEConnect for this 2 below:
        public const string NOTIFICATION_SEND = "kdeconnect.notification.send";
        public const string NOTIFICATION_RECEIVE = "kdeconnect.notification.receive";
        private HashMap<string, GLib.Notification> _notifications_receive;

        public string get_pkt_type () {
            return NOTIFICATION;
        }

        private NotificationHandler () {
            _notifications_receive = new HashMap<string, GLib.Notification>();
        }

        public static NotificationHandler instance () {
            return new NotificationHandler ();
        }

        public void use_device (Device dev) {
            debug ("Use device %s for notification.", dev.to_string ());
            dev.message.connect (this.message);
        }

        public void release_device (Device dev) {
            debug ("Release device %s", dev.to_string ());
            dev.message.disconnect (this.message);
        }

        /**
        * send_notification:
        *
        * Send a desktop notification to remote device.
        * notify-send "From Past to Present" "par <i>Jeremy Soule</i> sur l’album <i>The Elder Scrolls V: Skyrim</i>" -u normal -a Rhythmbox -i rhythmbox
        */
        public void send_notification (Device device, DBusMessage message, uint32 id) {

            Plugin.Notification plugin_notification = (Plugin.Notification)device.get_plugin (NOTIFICATION);

            if (device.is_capabality_activated (NOTIFICATION) == false ||
                plugin_notification.settings_send_is_active == false) {
                return;
            }

            Notification notification = new Notification.from_message (message, id);

            if (notification.app_name.length == 0) {
                debug ("app_name not defined, ignoring it. (body was: \"%s\")", notification.message_body);
                return;
            }

            if (plugin_notification.settings_apps_ids_banned.contains (notification.app_info.get_id ())) {
                debug ("App ID %s - %s - banned for device ID %s (%s), not sending the notification.",
                 notification.app_info.get_id (), notification.app_name, device.id, device.name);
                return;
            }

            int kdc_notification_id = (int)id + (int)notification.unix_time;
            string device_notification_id = id.to_string () + "_" + notification.unix_time.to_string ()
                                            + "_" + device.id;

            var builder = new Json.Builder ();
            builder.begin_object ();
            builder.set_member_name ("id");
            builder.add_int_value (kdc_notification_id);
            builder.set_member_name ("appName");
            builder.add_string_value (notification.app_name);
            builder.set_member_name ("title");
            builder.add_string_value (notification.summary);
            builder.set_member_name ("text");
            builder.add_string_value (notification.message_body);
            builder.set_member_name ("ticker");
            builder.add_string_value (notification.summary + "\n" + notification.message_body);
            builder.set_member_name ("isClearable");
            builder.add_int_value (1);

            // DateTime time = new DateTime.from_unix_local (notification.unix_time);
            bool there_is_payload = true;
            uint64 size = 0;
            uint16 port = 0;
            File icon_file;

            var icon_filename = this._get_app_icon_filename (notification.app_icon);
            if (icon_filename != null) {

                icon_file = File.new_for_path (icon_filename);
                try {
                    var fi = icon_file.query_info (FileAttribute.STANDARD_SIZE, FileQueryInfoFlags.NONE);
                    size = fi.get_size ();
                    debug ("Icon file size: %llu", size);
                    try {
                        FileInputStream input = icon_file.read ();
                        var listener = Core.instance ().transfer_manager.make_listener (out port);
                        if (listener == null) {
                            warning ("Could not allocate a listener on port %u", port);
                            there_is_payload = false;
                        } else {
                            debug ("Allocated listener on port %u", port);

                            var t = new UploadTransfer (device, listener, input, size);
                            Core.instance ().transfer_manager.push_job (t);
                            t.start_async.begin ();

                            //uint8[] contents;
                            string etag_out;
                            Bytes file_bytes = icon_file.load_bytes (null, out etag_out);

                            builder.set_member_name ("payloadHash");
                            builder.add_string_value (
                                Checksum.compute_for_bytes(
                                GLib.ChecksumType.MD5,
                                file_bytes));
                        }

                    } catch (Error e) {
                        warning ("Failed to open icon file at path %s: %s", icon_file.get_path (), e.message);
                        there_is_payload = false;
                    }

                } catch (Error e) {
                    warning ("Failed to obtain icon file size: %s", e.message);
                    there_is_payload = false;
                }
            } else {
                there_is_payload = false;
            }

            builder.end_object ();

            debug ("Notification to send:");
            debug ("\tdevice_id              : %s", device.id);
            debug ("\tdevice_notification_id : %s", device_notification_id);
            debug ("\tkdc_notification_id    : %i", kdc_notification_id);
            debug ("\tapp_name               : %s", notification.app_name);
            debug ("\tsummary                : %s", notification.summary);
            debug ("\tmessage_body           : %s", notification.message_body);
            debug ("\tapp_icon               : %s", notification.app_icon);
            debug ("\tsender                 : %s", notification.sender);
            if (there_is_payload) {
                debug ("\ticon_filename      : %s", icon_filename);
            }

            var packet = new Packet (
                NOTIFICATION,
                builder.get_root ().get_object ()
            );
            if (there_is_payload) {
                packet.payload = Packet.Payload () {
                    size = size,
                    port = port
                };
            }

            device.send (packet);
        }

        public void message (Device device, Packet pkt) {

            if (pkt.pkt_type != NOTIFICATION || device.is_capabality_activated (NOTIFICATION) == false) {
                return;
            }

            if (((Plugin.Notification)device.get_plugin (NOTIFICATION)).settings_receive_is_active == false) {
                return;
            }

            info ("Got notification packet.");
            //string notification_id = pkt.body.get_string_member ("id") + "_" + device.id;
            string notification_id = pkt.body.get_string_member ("id");

            // Dialer is handled by telephony plugin.
            if (notification_id.match_string ("com.android.dialer", false)) {
                return;
            }

            if (notification_id.contains ("android.messaging") == true) {
                return;
            }

            // If local device was not running, we can have SMS notification not yet handled.
            // It will store only if we have already a phone number associated to the name.
            // if (notification_id.contains ("android.messaging") == true) {
            //     if (pkt.body.has_member ("isCancel") == true &&
            //         pkt.body.get_boolean_member ("isCancel") == true) {
            //
            //             return;
            //     }
            //
            //     if(device.has_capability_handler (TelephonyHandler.TELEPHONY) == true) {
            //         TelephonyHandler telephony_handler = (TelephonyHandler)device.get_path_capability_handler (TelephonyHandler.TELEPHONY);
            //
            //         if (telephony_handler.last_sms_received_date_time == null) {
            //             telephony_handler.handle_sms_notification (device, pkt);
            //             return;
            //         }
            //
            //         DateTime current_date_time = new DateTime.now_utc ();
            //         TimeSpan time_span = current_date_time.difference (telephony_handler.last_sms_received_date_time);
            //         int minutes_from_last_sms_sent = (int)(time_span / 1000.0 / 1000.0 / 60.0);
            //         //print (">>%s | %s | %i \n",current_date_time.to_string () , telephony_handler.last_sms_received_date_time.to_string () , minutes_from_last_sms_sent);
            //         if (minutes_from_last_sms_sent > 1) {
            //             telephony_handler.handle_sms_notification (device, pkt);
            //         }
            //
            //         return;
            //     }
            // }

            // We already know this notifiction.
            if (_notifications_receive.has_key (notification_id) == true) {
                // If it's cancel on remote device, let's remove it on local device.
                if (pkt.body.has_member ("isCancel") == true &&
                    pkt.body.get_boolean_member ("isCancel") == true) {

                    debug ("Message cancel notification %s from device %s", notification_id, device.name);
                    try {
                        Core.instance ().application.withdraw_notification (notification_id);
                        _notifications_receive.unset (notification_id);
                    } catch (Error e) {
                        critical ("Error closing notification: %s from device %s ", e.message, device.name);
                    }
                }
                return;
            }

            if (pkt.body.has_member ("ticker") == false) {
                return;
            }

            string ticker = pkt.body.get_string_member ("ticker");
            if (ticker.length == 0) {
              return;
            }

            DateTime time = null;
            if (pkt.body.has_member ("time") == true) {
                int64 t = int64.parse (pkt.body.get_string_member ("time"));
                t /= 1000; // time in ms since Epoch, convert to seconds
                time = new DateTime.from_unix_local (t);
            }

            string device_name = (device.custom_name.length > 0) ? device.custom_name : device.name;
            string notif_summary = pkt.body.get_string_member ("appName") + "\u0040" + device_name;
            string notif_body = "<i>" + pkt.body.get_string_member ("title") + "</i>\n";
            notif_body += pkt.body.get_string_member ("text");

            if (pkt.body.has_member ("time") == true) {
                notif_summary = "%s at %s".printf (notif_summary, time.format ("%R"));
            }

            var notification = new GLib.Notification (notif_summary);
                notification.set_body (notif_body);
                notification.set_icon (new GLib.ThemedIcon (Utils.get_icon_name (device.device_type)));

            // Handle notification icon.
            if (pkt.payload != null) {
                string checksum_icon = Checksum.compute_for_string (
                    ChecksumType.MD5,
                    pkt.body.get_string_member ("appName") + "_" + notification_id);
                checksum_icon = "eos_icon_" + checksum_icon;
                // @TOCHECK Understand why icon_pathname is empty sometimes.
                //string icon_pathname ShareHandler.make_tmp_downloads_path (checksum_icon);
                string icon_pathname = "/tmp/" + checksum_icon;
                File icon_file = File.new_for_path (icon_pathname);

                if (icon_file.query_exists () == true) {
                    debug ("Icon for app-id %s is in cache: %s",
                           pkt.body.get_string_member ("appName") + "_" + notification_id, icon_pathname);
                    notification.set_icon (new FileIcon (icon_file));
                    _show_notification (device, notification, notification_id);
                } else {
                    DownloadTransfer t = ShareHandler.instance ().handle_icon (device, pkt, checksum_icon);
                    t.finished.connect(() => {
                        notification.set_icon (new FileIcon (icon_file));
                        _show_notification (device, notification, notification_id);
                    });
                    t.error.connect(() =>  {
                        _show_notification (device, notification, notification_id);
                    });
                }
            } else {
                _show_notification (device, notification, notification_id);
            }
        }

        private string? _get_app_icon_filename (string app_icon) {
            unowned Gtk.IconTheme theme = Gtk.IconTheme.get_default ();

            foreach (unowned AppInfo appinfo in AppInfo.get_all ()) {
                Icon? icon = appinfo.get_icon ();
                if (icon != null && icon is ThemedIcon) {
                    Gtk.IconInfo? iconinfo = theme.lookup_by_gicon (icon, 48, 0);
                    if (iconinfo != null  && icon.to_string () == app_icon) {
                        return iconinfo.get_filename ();
                    }
                }
            }

            return null;
        }

        private void _show_notification (Device device, GLib.Notification notification, string notification_id) {
            try {
                this._notifications_receive.@set (notification_id, notification);
                Core.instance ().application.send_notification (notification_id, notification);
            } catch (Error e) {
                warning ("Error: %s", e.message);
            }
        }

        // @TOCHECK: kdeconnect-android 1.7+ only, this is untested and not used yet.
        //
        //private Packet _make_reply_packet(string reply_id, string message_body ) {
        //    var builder = new Json.Builder ();
        //    builder.begin_object ();
        //        builder.set_member_name ("replyId");
        //            builder.add_string_value (reply_id);
        //        builder.set_member_name ("messageBody");
        //            builder.add_string_value ("test reply");
        //        builder.end_object ();
        //
        //    var packet = new Packet (
        //        NOTIFICATION_REPLY,
        //        builder.get_root ().get_object ()
        //    );
        //    return packet;
        //}
    }
}

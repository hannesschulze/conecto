/*-
 * Copyright (c) 2015 Wingpanel Developers (http://launchpad.net/wingpanel)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Library General Public License as published by
 * the Free Software Foundation, either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * gyan000 <gyan000 (at] ijaz.fr>
 *
 * Original code from:
 * http://bazaar.launchpad.net/~jconti/recent-notifications/gnome3/view/head:/src/recent-notifications.vala
 * And:
 * https://github.com/elementary/wingpanel-indicator-notifications/blob/master/src/Services/NotificationsMonitor.vala
 */
using Gee;

namespace MConnect {

    public class NotificationMonitor : Object {

        public signal void notification_received (DBusMessage message, uint32 id);
        public signal void notification_closed (Notification notification);

        private const string NOTIFY_IFACE = "org.freedesktop.Notifications";
        private const string NOTIFY_PATH = "/org/freedesktop/Notifications";
        private const string METHOD_CALL_MATCH_STRING = "eavesdrop='true',type='method_call',interface='org.freedesktop.Notifications'";
        private const string METHOD_RETURN_MATCH_STRING = "eavesdrop='true',type='method_return'";
        private const string ERROR_MATCH_STRING = "eavesdrop='true',type='error'";
        private DBusConnection _connection;
        private DBusMessage? awaiting_reply = null;
        private DBusNotificationsInterface? _notifications_iface = null;
        private  ArrayList<uint32>  _notifications_ids;

        public NotificationMonitor () {
            try {
                _notifications_ids = new ArrayList<uint32> ();
                _connection = Bus.get_sync (BusType.SESSION);
                add_rule (ERROR_MATCH_STRING);
                add_rule (METHOD_CALL_MATCH_STRING);
                add_rule (METHOD_RETURN_MATCH_STRING);
                _connection.add_filter (message_filter);
            } catch (Error e) {
                error ("%s\n", e.message);
            }

            try {
                _notifications_iface = Bus.get_proxy_sync (BusType.SESSION, NOTIFY_IFACE, NOTIFY_PATH);
            } catch (Error e) {
                error ("%s\n", e.message);
            }
        }

        private void add_rule (string rule) {
            var message = new DBusMessage.method_call ("org.freedesktop.DBus",
                                                       "/org/freedesktop/DBus",
                                                       "org.freedesktop.DBus",
                                                       "AddMatch");

            var body = new Variant.parsed ("(%s,)", rule);
            message.set_body (body);

            try {
                _connection.send_message (message, DBusSendMessageFlags.NONE, null);
            } catch (Error e) {
                warning ("%s\n", e.message);
            }
        }

        private DBusMessage? message_filter (DBusConnection con, owned DBusMessage message, bool incoming) {

            // if (message.get_interface () == NOTIFY_IFACE) {
                // print ("\n>>%s\n",  message.print ());
            // }

            // TODO Try to sync when we close notification.
            // if (incoming && message.get_interface () == NOTIFY_IFACE && message.get_message_type () == DBusMessageType.SIGNAL) {
            //     print ("\n>>%s\n",  message.print ());
            //     if (message.get_member () == "NotificationClosed") {
            //         var body = message.get_body ();
            //         if (body.n_children () != 1) {
            //             return message;
            //         }
            //         print ("\n>>%s\n", "1");
            //         var child = body.get_child_value (0);
            //         if (!child.is_of_type (VariantType.UINT32)) {
            //             return message;
            //         }
            //         print ("\n>>%s\n", "2");
            //         uint32 id = child.get_uint32 ();
            //
            //         Notification notification = new Notification.from_message (message, id);
            //         notification_closed (notification);
            //         return message;
            //     }
            // }

            if (incoming && message.get_interface () == NOTIFY_IFACE && message.get_message_type () == DBusMessageType.METHOD_CALL) {
                if (message.get_member () == "Notify") {
                    try {
                        awaiting_reply = message.copy ();
                    } catch (Error e) {
                        warning (e.message);
                    }
                } else if (message.get_member () == "CloseNotification") {
                    var body = message.get_body ();
                    if (body.n_children () != 1) {
                        return message;
                    }

                    var child = body.get_child_value (0);
                    if (!child.is_of_type (VariantType.UINT32)) {
                        return message;
                    }

                    uint32 id = child.get_uint32 ();
                    Idle.add (() => {
                        return false;
                    });
                }

                return null;
            } else if (awaiting_reply != null && awaiting_reply.get_serial () == message.get_reply_serial ()) {
                if (message.get_message_type () == DBusMessageType.METHOD_RETURN) {
                    var body = message.get_body ();
                    if (body.n_children () != 1) {
                        return message;
                    }

                    var child = body.get_child_value (0);
                    if (!child.is_of_type (VariantType.UINT32)) {
                        return message;
                    }

                    uint32 id = child.get_uint32 ();
                    try {
                        var copy = awaiting_reply.copy ();
                        Idle.add (() => {
                            // @TOCHECK Did not understand yet why notif' are sent twice.
                            if (_notifications_ids.contains (id) == false) {
                                notification_received (copy, id);
                                _notifications_ids.add (id);
                            } else {
                                _notifications_ids.remove (id);
                            }
                            return false;
                        });
                    } catch (Error e) {
                        warning (e.message);
                    }

                    awaiting_reply = null;
                }
                else if (message.get_message_type () == DBusMessageType.ERROR) {
                    awaiting_reply = null;
                }
            }

            return message;
        }
    }
}

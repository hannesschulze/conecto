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
using EOSConnect;

namespace MConnect {

    public class DeviceManager : GLib.Object {

        public signal void found_new_device (Device dev);
        public signal void device_connected (Device dev);
        public signal void device_deconnected (Device dev);
        public signal void device_pair_changed (Device dev);
        public signal void device_capability_added (Device dev, string capability, PacketHandlerInterface handler);

        public const string DEVICES_CACHE_FILE = "devices";

        public DeviceManager () {
            debug ("Device manager.");
            var notification_monitor = new NotificationMonitor ();
            notification_monitor.notification_received.connect (on_local_notification_received);
            notification_monitor.notification_closed.connect (on_local_notification_closed);
        }

        /**
         * Obtain path to devices cache file
         */
        private string get_cache_file () {
            var cache_file = Path.build_filename (Core.get_cache_dir (), DEVICES_CACHE_FILE);
            debug ("Cache file: %s", cache_file);

            // make sure that cache dir exists
            DirUtils.create_with_parents (Core.get_cache_dir (), 0700);

            return cache_file;
        }

        /**
         * Load known devices from cache and attempt pairing.
         */
        public void load_cache () {
            var cache_file = this.get_cache_file ();

            debug ("Try loading devices from device cache %s", cache_file);

            var kf = new KeyFile ();
            try {
                kf.load_from_file (cache_file, KeyFileFlags.NONE);

                string[] groups = kf.get_groups ();

                foreach (string group in groups) {
                    var dev = Device.new_from_cache (kf, group);
                    if (dev != null) {
                        debug ("device %s from cache", dev.to_string ());
                        handle_device (dev);
                    }
                }
            } catch (Error e) {
                debug ("Error loading cache file: %s", e.message);
            }
        }

        public void handle_discovered_device (DiscoveredDevice discovered_dev) {
            debug ("Found device: %s", discovered_dev.to_string ());
            var new_dev = new Device.from_discovered_device (discovered_dev);
            handle_device (new_dev);
        }

        public void handle_device (Device new_dev) {
            try {
                var is_new = false;
                string unique = new_dev.to_unique_string ();
                debug ("Device key: %s", unique);

                if (Core.instance ().devices_map.has_key (unique) == false) {
                    debug ("Adding new device with key: %s", unique);
                    Core.instance ().devices_map.@set (unique, new_dev);
                    is_new = true;
                } else {
                    debug ("Device %s already present", unique);
                    this.device_connected (Core.instance ().devices_map.@get (unique));
                }

                var dev = Core.instance ().devices_map.@get (unique);

                // Notify everyone that a new device appeared
                if (is_new) {
                    // Make sure that this happens before we update device data so that
                    // all subscribeds of found_new_device() signal have a chance to
                    // setup eveything they need.
                    this.found_new_device (dev);
                    dev.capability_added.connect (this.device_capability_added_cb);
                    dev.capability_removed.connect (this.device_capability_removed_cb);
                }

                // Update device information
                dev.update_from_device (new_dev);

                debug ("Allowed? %s", dev.allowed.to_string ());
                // check if device is whitelisted in configuration
                if (!dev.allowed && device_allowed_in_config (dev)) {
                    dev.allowed = true;
                }

                this.update_cache ();

                if (dev.allowed) {
                    this.activate_device (dev);
                } else {
                    warning ("Skipping device %s activation, device not allowed", dev.to_string ());
                }
            }
            catch (Error e) {
                warning ("Error: %s", e.message);
            }
        }

        /**
         * allow_device:
         * @path: device object path
         *
         * Allow given device
         */
        public void allow_device (Device dev) {
            dev.allowed = true;
            update_cache ();
            activate_device (dev);
        }

        /**
         * disallow_device:
         * @path: device object path
         *
         * Disallow given device
         */
        public void disallow_device (Device dev) {
            dev.allowed = false;
            update_cache ();
        }

        private void on_local_notification_received (DBusMessage message, uint32 id) {
            try {
                var notification = new Notification.from_message (message, id);
                if (!notification.get_is_valid () || notification.app_name in Notification.EXCEPTIONS) {
                    return;
                }

                foreach (Device device in Core.instance ().devices_map.values) {
                    if (device.has_capability_handler (NotificationHandler.NOTIFICATION)) {
                        NotificationHandler notification_handler = (NotificationHandler)device.get_path_capability_handler(
                            NotificationHandler.NOTIFICATION);

                        notification_handler.send_notification (device, message, id);
                    }
                }
            } catch (Error e) {
                warning ("Error: %s", e.message);
            }
        }

        private void on_local_notification_closed (Notification notification) {
            try {

                print ("\n>>%S\n", notification.id);

                // var notification = new Notification.from_message (message, id);
                // Core.instance ().application.withdraw_notification (notification_id);

                // if (!notification.get_is_valid () || notification.app_name in Notification.EXCEPTIONS) {
                //     return;
                // }
                //
                // foreach (Device device in Core.instance ().devices_map.values) {
                //     if (device.has_capability_handler (NotificationHandler.NOTIFICATION)) {
                //         NotificationHandler notification_handler = (NotificationHandler)device.get_path_capability_handler(
                //             NotificationHandler.NOTIFICATION);
                //
                //         notification_handler.send_notification (device, message, id);
                //     }
                // }
            } catch (Error e) {
                warning ("Error: %s", e.message);
            }
        }

        /**
         * Update contents of device cache
         */
        private void update_cache () {
            try {
                if (Core.instance ().devices_map.size == 0)
                    return;

                var kf = new KeyFile ();

                foreach (var dev in Core.instance ().devices_map.values) {
                    dev.to_cache (kf, dev.name);
                }

                debug("Saving to cache.");
                FileUtils.set_contents (get_cache_file (), kf.to_data ());
            } catch (FileError e) {
                warning ("Failed to save to cache file %s: %s", get_cache_file (), e.message);
            } catch (Error e) {
                warning ("Error: %s", e.message);
            }
        }

        private void activate_device (Device dev) {
            info ("Activating device %s, active: %s", dev.to_string (), dev.is_active.to_string ());

            if (!dev.is_active) {
                dev.paired.connect (this.device_paired);
                dev.disconnected.connect (this.device_disconnected);
                dev.activate ();
            }
        }

        /**
         * device_allowed_in_config:
         * @dev device
         *
         * Returns true if a matching device is enabled via configuration file.
         */
        private bool device_allowed_in_config (Device dev) throws Error {
            if (dev.allowed)
                return true;

            // var core = Core.instance ();

            // var in_config = core.config.is_device_allowed (dev.name, dev.device_type);
            // return in_config;
            return true;
        }

        private void device_paired (Device dev, bool status) {
            info ("Device %s pair status change: %s", dev.to_string (), status.to_string ());

            update_cache ();

            if (status == false) {
                // we're no longer interested in paired signal
                dev.paired.disconnect (this.device_paired);
                // we're not paired anymore, deactivate if needed
                dev.deactivate ();
            }

            device_pair_changed (dev);
        }

        private void device_capability_added_cb (Device dev, string cap) {
            try {
                info ("Capability %s added to device %s", cap, dev.to_string ());

                if (dev.has_capability_handler (cap)) {
                    return;
                }

                var core = Core.instance ();
                var h = core.handlers.get_capability_handler (cap);
                if (h != null) {
                    dev.register_capability_handler (cap, h);
                    device_capability_added (dev, cap, h);
                } else {
                    warning ("No handler for capability %s", cap);
                }
            }
            catch (Error e) {
                warning ("Error: %s", e.message);
            }
        }

        private void device_capability_removed_cb (Device dev, string cap) {
            info ("Capability %s removed from device %s", cap, dev.to_string ());
        }

        private void device_disconnected (Device dev) {
            debug ("Device %s got disconnected", dev.to_string ());

            dev.paired.disconnect (this.device_paired);
            dev.disconnected.disconnect (this.device_disconnected);
            device_deconnected (dev);
        }
    }
}

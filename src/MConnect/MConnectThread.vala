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

namespace MConnect {

    public class MConnectThread : Object{

        public signal void devices_updated ();
        public signal void devices_status_updated ();

        private Core core = null;
        private Discovery discovery = null;
        private DeviceManager manager = null;
        private TransferManager transfer = null;
        public Granite.Application application;
        public HashMap<string, Device> devices_map;
        GLib.Settings main_settings;

        public MConnectThread (Granite.Application application,
                              HashMap<string, Device> devices_map,
                              GLib.Settings main_settings) {
            this.application = application;
            this.devices_map = devices_map;
            this.main_settings = main_settings;
            discovery = new Discovery ();
            manager = new DeviceManager ();
            transfer = new TransferManager ();
        }

        public int run () {

            MainLoop loop = new MainLoop ();
            try {
                core = Core.instance ();
                if (core == null)
                    error ("Unable to initialize core.");

                core.transfer_manager = transfer;
                core.application = application;
                core.devices_map = devices_map;
                core.main_settings = main_settings;

                discovery.device_found.connect ((disc, discovered_device) => {
                    manager.handle_discovered_device (discovered_device);
                });


                manager.found_new_device.connect ((device)  => {
                    devices_updated ();
                });

                manager.device_connected.connect ((device)  => {
                    devices_updated ();
                });

                manager.device_deconnected.connect ((device)  => {
                    devices_updated ();
                });

                manager.device_pair_changed.connect ((device)  => {
                    devices_updated ();
                });


                // BatteryPlugingListBoxRow will call signal connect ()
                ((BatteryHandler)Core.instance ().handlers.get_capability_handler (BatteryHandler.BATTERY))
                    .battery_update.connect (() => { devices_status_updated (); });

                // TelephonyPluginListBoxRow will call signal telephony_update ()
                ((TelephonyHandler)Core.instance ().handlers.get_capability_handler (TelephonyHandler.TELEPHONY))
                    .telephony_update.connect (() => { devices_status_updated (); });

                manager.load_cache ();

                discovery.listen ();

                Bus.own_name (BusType.SESSION, "com.github.hannesschulze.conecto", BusNameOwnerFlags.NONE,
                  on_bus_aquired,
                  () => {},
                  () => stderr.printf ("Could not aquire name\n"));


                loop.run ();
            } catch (Error e) {
                warning ("Error: %s\n", e.message);
            }

            return 0;
        }

        public void on_bus_aquired (DBusConnection conn) {
            try {
                // Share service.
                conn.register_object ("/com/github/hannesschulze/conecto/share", new ShareHandlerProxy ());
            } catch (IOError e) {
                warning ("Could not register service.\n");
            }
        }

        public void shutdown () {
            Process.exit (0);
        }
    }
}

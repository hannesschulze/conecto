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
 * gyan000 <gyan000 (at] ijaz.fr>
 */
using EOSConnect.Plugin;
using EOSConnect.Plugin.Windows;
using EOSConnect.Widgets;
using Gee;
using MConnect;
using Unity;

namespace EOSConnect {

    public class App : Granite.Application {

        public const string APP_NAME="eos-connect";
        public const string GSETTINGS_SCHEMA_ID="com.github.gyan000.eos-connect";
        public const string GSETTINGS_SCHEMA_PATH="/com/github/gyan000/eos-connect";
        public HashMap<string, Device> devices_map;
        public ContactsInterface contacts_interface;
        private GLib.Settings main_settings;
        private SMSHistory sms_history_view;
        private MainWindow main_window;
        private LauncherEntry launcher_entry;

        public App () {
           devices_map = new HashMap<string, Device> ();
           main_settings = new GLib.Settings (GSETTINGS_SCHEMA_ID);
        }

        construct {
            application_id = App.GSETTINGS_SCHEMA_ID;
            flags = ApplicationFlags.FLAGS_NONE;
            program_name = "EOS Connect";
            build_version = "0.5";
        }

        public override void activate ()
        {
            Contractor.clean_contractor_directory.begin ();
            MConnectThread mconnect_thread = new MConnectThread (this, devices_map, main_settings);

            launcher_entry = LauncherEntry.get_for_desktop_id (GSETTINGS_SCHEMA_ID + ".desktop");

            sms_history_view = new SMSHistory (main_settings, devices_map);
            // sms_history_view.delete_event.connect ((event) => {
            //     // sms_history_view.iconify ();
            //     return true;
            // });

            main_window = new MainWindow (this, main_settings, sms_history_view);
            main_window.show_all ();
            main_window.delete_event.connect ((event) => {
                // When from launcher menu is clicked.
                if (event.send_event == (int8)0) {
                    foreach (var device_entry in devices_map.entries) {
                        Contractor.destroy_contract (device_entry.value);
                    }
                    main_window.destroy ();
                } else { // When close button from the main window is clicked.
                    main_window.iconify ();
                }
                return true;
            });

            init_sms_store.begin (() => {
                init_sms_history_tab.begin ();
            });


            mconnect_thread.devices_updated.connect (() => {
                update_launcher_entry.begin  ();
                main_window.update_ui (devices_map);
                sms_history_view.update_available_device_combobox ();
            });

            mconnect_thread.devices_status_updated.connect (() => {
                update_launcher_entry.begin ();
                sms_history_view.update_available_device_combobox ();
            });

            shutdown.connect (() => {
                mconnect_thread.shutdown();
            });

            new Thread<int> ("MConnect Thread", mconnect_thread.run);
        }

        public static int main (string[] args) {

            // Needed for mousepad protocol handler.
            Gdk.init (ref args);

            // Needed for clipboard sharing.
            Gtk.init (ref args);

            if (!Thread.supported ()) {
                error ("Cannot run without thread support.\n");
            }

             message ("Report any issues/bugs you might find to https://github.com/gyan000/EOSConnect/issues");

            var application = new App ();
            return application.run (args);
        }

        private async void init_sms_store () {
            SMSStore.instance (main_settings);
        }

        private async void init_sms_history_tab () {

            SimpleAction simple_action = new SimpleAction ("telephony-open-sms-tab", null);
            simple_action.activate.connect (() => {
                main_window.main_stack.set_visible_child_name ("sms_history_view");
            });

            this.add_action (simple_action);

            SMSStore.instance ().sms_handled.connect ( (sms) => {
                sms_history_view.update_message_ui_adding_sms (sms);
            });

            SMSStore.instance ().contacts_updated.connect ( (sms_contact_map) => {
                sms_history_view.update_ui (sms_contact_map);
            });

            SMSStore.instance ().contact_updated.connect ((sms_contact) => {
                sms_history_view.update_list_box_ui (sms_contact);
            });

            SMSStore.instance ().refresh_contact_list ();

            contacts_interface = new EContacts ();
            contacts_interface.load_contacts ();
            contacts_interface.contact_loaded.connect ((contact ) => {
                SMSStore.instance ().merge_contact (contact);
            });

            sms_history_view.contacts_interface = contacts_interface;
        }

        private async void update_launcher_entry () {

            if (launcher_entry.quicklist == null) {
                launcher_entry.quicklist =  new Dbusmenu.Menuitem ();
            }

            foreach (var device_entry in devices_map.entries) {
                bool to_add = true;
                bool to_remove = false;

                if (device_entry.value.is_paired == false || device_entry.value.is_active == false) {
                    to_remove = true;
                }


                foreach (var device_menuitem in launcher_entry.quicklist.get_children ()) {

                    if (((DeviceMenuitem)device_menuitem).id == device_entry.value.device_num) {
                        ((DeviceMenuitem)device_menuitem).update_ui ();
                        to_add = false;

                        if (to_remove == true) {
                            launcher_entry.quicklist.child_delete (device_menuitem);
                            Contractor.destroy_contract (device_entry.value);
                        }
                    }
                }

                if (to_add == true && to_remove == false) {
                    launcher_entry.quicklist.child_append (
                        new DeviceMenuitem.with_device (device_entry.value, main_window));
                }

                // var device_menuitem = launcher_entry.quicklist.find_id (device_entry.value.device_num);
                // @TOCHECK This condition is not working as expected:
                // if (device_menuitem is Dbusmenu.Menuitem) {
                //     print ("\n[UPDATE DEVICE]================> %i - %s\n", device_entry.value.device_num, device_entry.value.custom_name);
                //      ((DeviceMenuitem)device_menuitem).update_ui ();
                // } else {
                //     print ("\n[ADD DEVICE]================> %i - %s\n", device_entry.value.device_num, device_entry.value.custom_name);
                //     launcher_entry.quicklist.child_append (new DeviceMenuitem.with_device (device_entry.value));
                // }
            }
        }
    }
}

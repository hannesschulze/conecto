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
using Gee;
using EOSConnect;
using EOSConnect.Plugin;
using EOSConnect.Plugin.Views;
using MConnect;

namespace EOSConnect.Plugin.Windows {

    public class SMSHistory : Gtk.Stack {

        public GLib.Settings main_settings { get; construct; }
        public HashMap<string, Device> devices_map { get; construct; }
        public ContactsInterface contacts_interface;

        private Popover.SMSNewMessageNewContact popover_new_sms;
        private Widgets.SMSContactListBox sms_contact_list_box;
        private Gtk.Stack stack;
        private string latest_contact_selected = null;

        public SMSHistory (GLib.Settings main_settings, HashMap<string, Device> devices_map) {
            Object (
                main_settings: main_settings,
                devices_map: devices_map
            );
        }

        construct {

            var scrolled_window = new Gtk.ScrolledWindow (null, null);
            scrolled_window.expand = true;
            scrolled_window.hscrollbar_policy = Gtk.PolicyType.NEVER;

            sms_contact_list_box = new Widgets.SMSContactListBox ();
            sms_contact_list_box.width_request = 180;
            scrolled_window.add (sms_contact_list_box);

            var sidebar_grid = new Gtk.Grid ();
            sidebar_grid.orientation = Gtk.Orientation.VERTICAL;
            sidebar_grid.add (scrolled_window);

            stack = new Gtk.Stack ();
            stack.set_transition_type (Gtk.StackTransitionType.CROSSFADE);

            var paned = new Gtk.Paned (Gtk.Orientation.HORIZONTAL);
            paned.pack1 (sidebar_grid, true, false);
            paned.pack2 (stack, true, false);

            margin = 12;
            add (paned);

            sms_contact_list_box.row_selected.connect (sms_contact_list_box_selected);

            sms_contact_list_box.show_all ();
            scrolled_window.show_all ();
        }

        public void update_list_box_ui (SMSContact sms_contact) {
            int count = 0;

            sms_contact_list_box.@foreach (() => {
                Widgets.SMSContactListBoxRow sms_contact_list_box_row = (Widgets.SMSContactListBoxRow)sms_contact_list_box.get_row_at_index (count);
                if (sms_contact_list_box_row.sms_contact.phone_number == sms_contact.phone_number) {
                    sms_contact_list_box_row.update_ui ();
                }
                count++;
            });
        }

        public void update_ui (HashMap<string, SMSContact> sms_contact_map) {
            foreach (var entry in sms_contact_map.entries) {
                add_sms_history_message_view (entry.value);

                bool contact_already_in_list_box = false;
                int count = 0;

                sms_contact_list_box.@foreach (() => {
                    Widgets.SMSContactListBoxRow sms_contact_list_box_row = (Widgets.SMSContactListBoxRow)sms_contact_list_box.get_row_at_index (count);
                    if (sms_contact_list_box_row.sms_contact.phone_number == entry.value.phone_number) {
                        contact_already_in_list_box = true;
                        ((SMSMessagesView)stack.get_child_by_name (entry.value.phone_number)).update_combo_box_devices ();
                    }
                    count++;
                });

                if (contact_already_in_list_box == false) {
                    sms_contact_list_box.insert (new Widgets.SMSContactListBoxRow (entry.value), count);
                }
            }

            // First time we populate the list box.
            if (sms_contact_list_box.get_selected_row () == null && latest_contact_selected == null) {
                sms_contact_list_box.select_row (sms_contact_list_box.get_row_at_index (0));
                latest_contact_selected = ((Widgets.SMSContactListBoxRow)sms_contact_list_box.get_row_at_index (0)).sms_contact.phone_number;
            }

            show_all ();
        }

        public void update_message_ui_adding_sms (SMS sms) {
            if (sms.sms_contact != null) {
                ((SMSMessagesView)stack.get_child_by_name (sms.sms_contact.phone_number)).add_message_in_textview (sms);
            }
            update_available_device_combobox ();
        }

        public void update_available_device_combobox () {
            stack.get_children ().foreach ((entry) => {
                SMSMessagesView sms_view = (SMSMessagesView)entry;
                sms_view.update_combo_box_devices ();
            });
        }

        private void add_sms_history_message_view (SMSContact sms_contact) {

            if (stack.get_child_by_name (sms_contact.phone_number) == null) {
                debug ("Adding SMSMessagesView Widget for contact '%s'".printf (sms_contact.contact_name));

                SMSMessagesView sms_message_view = new SMSMessagesView (sms_contact, sms_contact_list_box, devices_map);
                sms_message_view.update_combo_box_devices ();
                stack.add_named (sms_message_view, sms_contact.phone_number);
            }
        }

        private void sms_contact_list_box_selected (Gtk.ListBoxRow? sms_contact_item) {
           SMSContact? sms_contact = null;

           if (sms_contact_item != null) {
               sms_contact = ((Widgets.SMSContactListBoxRow)sms_contact_item).sms_contact;
               latest_contact_selected = sms_contact.phone_number;
               stack.set_visible_child_name (sms_contact.phone_number);
           }
       }

       public void handle_new_sms_button (Gtk.Button new_sms_button) {
            if (popover_new_sms == null) {
               var popover_new_sms = new Popover.SMSNewMessageNewContact (new_sms_button, contacts_interface, devices_map);
               popover_new_sms.update_combo_box_devices ();
               popover_new_sms.show_all ();

               popover_new_sms.destroy.connect (() => {
                   popover_new_sms = null;
               });
           }
       }
    }
}

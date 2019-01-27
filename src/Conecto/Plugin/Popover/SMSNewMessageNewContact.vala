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
using Conecto;
using Conecto.Plugin.Widgets;
using Gee;
using MConnect;

namespace Conecto.Plugin.Popover {

    public class SMSNewMessageNewContact : Gtk.Popover {

        public ContactsInterface contacts_interface { get; construct; }
        public HashMap<string, Device> devices_map { get; construct; }

        private Gtk.ListBox list_box;
        private SMSTextEntry text_entry;

        public SMSNewMessageNewContact (
            Gtk.Widget? relative_to,
            ContactsInterface contacts_interface,
            HashMap<string, Device> devices_map
        ) {

            Object (
               relative_to: relative_to,
               modal: true,
               position: Gtk.PositionType.BOTTOM,
               contacts_interface: contacts_interface,
               devices_map: devices_map
           );
        }

        construct {
            height_request = 200;
            width_request = 400;

            list_box = new Gtk.ListBox ();

            text_entry = new SMSTextEntry (devices_map);
            text_entry.entrytext_view.vexpand = true;
            text_entry.message_to_send.connect ((message, device) => { send_message (message, device); });

            var main_stack = new Gtk.Stack ();
            main_stack.expand = true;
            main_stack.transition_type = Gtk.StackTransitionType.SLIDE_LEFT_RIGHT;
            main_stack.add_titled (new SelectContactStackView (contacts_interface, list_box), "select_contact_view", _("Contacts"));
            main_stack.add_titled (text_entry, "text_entry_view", _("Message"));

            var main_stackswitcher = new Gtk.StackSwitcher ();
            main_stackswitcher.set_stack (main_stack);
            main_stackswitcher.halign = Gtk.Align.CENTER;
            main_stackswitcher.homogeneous = true;

            var grid = new Gtk.Grid ();
            grid.orientation = Gtk.Orientation.VERTICAL;
            grid.margin = 5;
            grid.add (main_stackswitcher);
            grid.add (main_stack);

            add (grid);
        }

        public void update_combo_box_devices () {
            text_entry.update_combo_box_devices ();
        }

        private void send_message (string message, Device device) {
            SMS sms = new SMS (
                message,
                SMS.FROM_ME,
                SMS.FROM_TYPE_CONECTO,
                new DateTime.now_utc ());

            if (device.has_capability_handler (TelephonyHandler.TELEPHONY) ) {
                int count = 0;

                list_box.@foreach (() => {
                    ContactListBoxRow clbw = (ContactListBoxRow)list_box.get_row_at_index (count);

                    ((TelephonyHandler)device.get_path_capability_handler(TelephonyHandler.TELEPHONY))
                        .send_sms (device, clbw.contact.international_phone_number, message);

                    SMSStore.instance ().add_sms (
                        device,
                        sms,
                        new SMSContact (
                            clbw.contact.international_phone_number,
                            clbw.contact.name,
                            device.id,
                            "",
                            clbw.contact.name
                        ));
                    count++;
                });
            }
        }

        private class SelectContactStackView : Gtk.Box {
            public SelectContactStackView (ContactsInterface contacts_interface, Gtk.ListBox list_box) {
                var scrolled_window = new Gtk.ScrolledWindow (null, null);
                scrolled_window.expand = true;
                scrolled_window.hscrollbar_policy = Gtk.PolicyType.NEVER;

                list_box.margin = 5;
                list_box.selection_mode = Gtk.SelectionMode.NONE;

                scrolled_window.add (list_box);

                var search_entry = new Gtk.SearchEntry();
                search_entry.margin = 5;
                search_entry.placeholder_text = _("Search a contact or enter a phone number");

                var list_store = new Gtk.ListStore(2, typeof(string), typeof(Contact));

                Gtk.TreeIter iter;
                foreach (Contact contact in contacts_interface.contacts) {
                    list_store.append(out iter);
                    list_store.set(iter, 0, contact.name, 1, contact);
                }

                var entry_completion = new Gtk.EntryCompletion();
                entry_completion.model = list_store;
                entry_completion.text_column = 0;
                entry_completion.popup_completion = true;

                search_entry.set_completion(entry_completion);

                entry_completion.match_selected.connect ( (model, iter) => {
                    GLib.Value name;
                    GLib.Value contact;
                    list_store.get_value (iter, 0, out name);
                    list_store.get_value (iter, 1, out contact);

                    bool insert = true;
                    int count = 0;

                    list_box.@foreach (() => {
                        ContactListBoxRow clbw = (ContactListBoxRow)list_box.get_row_at_index (count);
                        if (((Contact)contact).international_phone_number == clbw.contact.international_phone_number) {
                            insert = false;
                        }
                    });

                    if (insert == true) {
                        list_box.insert (new ContactListBoxRow ((Contact)contact), -1);
                        search_entry.text = "";
                    }

                    return true;
                });


                search_entry.activate.connect ( () => {

                    // TODO - validate phone phone number
                    var default_contact = new Contact (
                        search_entry.get_text (),
                        search_entry.get_text (),
                        ""
                    );

                    list_box.insert (new ContactListBoxRow (default_contact), -1);
                    search_entry.text = "";
                });

                // left, top, width, height
                var grid = new Gtk.Grid ();
                grid.margin = 5;
                grid.attach (search_entry,    0, 0, 1, 1);
                grid.attach (scrolled_window, 0, 1, 1, 2);

                add (grid);
            }
        }

        private class ContactListBoxRow : Gtk.ListBoxRow {
            public Contact contact;

            public ContactListBoxRow (Contact contact) {

                this.contact = contact;

                var grid = new Gtk.Grid ();
                grid.margin = 6;
                grid.column_spacing = 6;

                var avatar = new Granite.Widgets.Avatar ();
                if (contact.photo_uri.length > 10) {
                    try {
                        var size = 32 * get_style_context ().get_scale ();
                        var avatar_pixbuf = new Gdk.Pixbuf.from_file_at_scale (
                            File.new_for_uri(contact.photo_uri).get_path (), size, size, false);
                        avatar.pixbuf = avatar_pixbuf;
                    } catch (Error e) {
                        warning ("Error: %s", e.message);
                        avatar.show_default (32);
                    }
                } else {
                    avatar.show_default (32);
                }

                var contact_name_label = new Gtk.Label (contact.name);
                contact_name_label.get_style_context ().add_class (Granite.STYLE_CLASS_H3_LABEL);
                contact_name_label.ellipsize = Pango.EllipsizeMode.END;
                contact_name_label.halign = Gtk.Align.START;
                contact_name_label.valign = Gtk.Align.START;

                var contact_phone_number = new Gtk.Label (contact.international_phone_number);
                contact_phone_number.margin_top = 2;
                contact_phone_number.use_markup = true;
                contact_phone_number.ellipsize = Pango.EllipsizeMode.END;
                contact_phone_number.halign = Gtk.Align.START;
                contact_phone_number.valign = Gtk.Align.START;

                var box = new Gtk.Box (Gtk.Orientation.HORIZONTAL, 0);
                box.pack_start (contact_phone_number, false, false, 0);

                grid.attach (avatar,             0, 0, 1, 2);
                grid.attach (contact_name_label, 1, 0, 1, 1);
                grid.attach (box,                1, 1, 1, 1);

                add (grid);

                show_all ();
            }
        }
    }
}

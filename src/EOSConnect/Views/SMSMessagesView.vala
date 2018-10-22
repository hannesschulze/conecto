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
using EOSConnect;
using EOSConnect.Plugin;
using EOSConnect.Plugin.Widgets;
using Gee;
using MConnect;

namespace EOSConnect.Views {

    public class SMSMessagesView : Gtk.Grid {

        public MainWindow main_window { get; construct; }
        // public GLib.Settings main_settings { get; construct; }

        public HashMap<string, Device> devices_map { get; construct; }
        public SMSContact sms_contact { get; construct; }
        public Widgets.SMSContactListBox device_list_box { get; construct; }

        private SMSTextEntry text_entry;
        private DateTime first_message_date_time_utc;
        private Gtk.ScrolledWindow scrolled_window ;
        private Gtk.TextIter text_iter;
        private Gtk.TextBuffer text_message_buffer;
        private Gtk.TextView text_view;
        private Gtk.TextMark text_mark_auto_scroll;

        private int last_message_day = 0;

        public MainWindow main_window { get; construct; }
        public GLib.Settings main_settings { get; construct; }
        //
        // public MainSettingsView (MainWindow main_window, GLib.Settings main_settings) {
        //     Object (
        //         main_settings: main_settings,
        //         main_window: main_window
        //     );
        // }


        public SMSMessagesView (
            MainWindow main_window,
            SMSContact sms_contact,
            Widgets.SMSContactListBox device_list_box,
            HashMap<string, Device> devices_map
        ) {

            Object (
                main_window: main_window,
                sms_contact: sms_contact,
                device_list_box: device_list_box,
                devices_map: devices_map
            );
        }

        construct {
            orientation = Gtk.Orientation.VERTICAL;

            scrolled_window = new Gtk.ScrolledWindow (null, null);
            scrolled_window.expand = true;
            scrolled_window.hscrollbar_policy = Gtk.PolicyType.NEVER;

            Gtk.TextTag text_tag_day = new Gtk.TextTag ("day");
            text_tag_day.scale = Pango.Scale.SMALL;
            text_tag_day.justification = Gtk.Justification.CENTER;
            text_tag_day.pixels_below_lines = 10;
            text_tag_day.pixels_above_lines = 10;
            text_tag_day.foreground = "black";

            Gtk.TextTag text_tag_msg = new Gtk.TextTag ("msg");
            text_tag_msg.foreground = "black";
            text_tag_msg.justification  = Gtk.Justification.LEFT;

            Gtk.TextTag text_tag_my_msg = new Gtk.TextTag ("my-msg");
            text_tag_my_msg.foreground = "black";
            text_tag_my_msg.left_margin = 150;
            text_tag_my_msg.justification  = Gtk.Justification.LEFT;

            Gtk.TextTag text_tag_time_msg = new Gtk.TextTag ("time-msg");
            text_tag_time_msg.justification  = Gtk.Justification.LEFT;
            text_tag_time_msg.foreground = "red";

            Gtk.TextTag text_tag_my_time_msg = new Gtk.TextTag ("my-time-msg");
            text_tag_my_time_msg.left_margin = 150;
            text_tag_my_time_msg.foreground = "blue";
            text_tag_my_time_msg.justification  = Gtk.Justification.LEFT;

            Gtk.TextTagTable text_tag_table = new Gtk.TextTagTable ();
            text_tag_table.add (text_tag_day);
            text_tag_table.add (text_tag_my_msg);
            text_tag_table.add (text_tag_msg);
            text_tag_table.add (text_tag_time_msg);
            text_tag_table.add (text_tag_my_time_msg);

            text_mark_auto_scroll = new Gtk.TextMark ("auto-scroll");

            text_message_buffer = new Gtk.TextBuffer (text_tag_table);
            text_message_buffer.get_iter_at_offset (out text_iter, 0);
            text_message_buffer.add_mark (text_mark_auto_scroll, text_iter);

            text_view = new Gtk.TextView ();
            text_view.set_wrap_mode (Gtk.WrapMode.WORD);
            text_view.editable = false;
            text_view.cursor_visible = false;
            text_view.buffer = text_message_buffer;
            text_view.top_margin = 5;
            text_view.bottom_margin = 5;
            text_view.left_margin = 5;
            text_view.right_margin = 5;

            int i = 0;
            foreach (SMS sms in SMSStore.instance ().get_latest_sms_message (sms_contact)) {
              if (i++ == 0) {
                first_message_date_time_utc = sms.date_time;
              }
              add_message_in_textview (sms);
            }

            scrolled_window.add (text_view);

            text_entry = new SMSTextEntry (devices_map);
            text_entry.message_to_send.connect ((message, device) => { send_message (message, device); });

            add (scrolled_window);
            add (text_entry);

            show_all ();

            scrolled_window.edge_overshot.connect ((pos) => {
              if( pos == Gtk.PositionType.TOP ) {
                foreach (SMS sms in SMSStore.instance ().get_previous_sms_message (sms_contact, first_message_date_time_utc)) {
                    first_message_date_time_utc = sms.date_time;
                    text_message_buffer.get_iter_at_line (out text_iter, 0);
                    add_message_in_textview (sms, true);
                }
                text_message_buffer.get_iter_at_line (out text_iter, text_message_buffer.get_line_count ());
              }
            }) ;
        }

        public void update_ui_adding_sms (SMS sms) {
            add_message_in_textview (sms);
        }

        public void update_combo_box_devices () {
            text_entry.update_combo_box_devices ();
        }

        public void add_message_in_textview (SMS sms, bool adding_old_sms = false) {
            if (last_message_day != sms.date_time.get_day_of_week ()) {
                string msg_datetime_format = "------------  " + sms.date_time.format ("%A %e %B %Y") + "  ------------";
                text_message_buffer.insert_with_tags_by_name (ref text_iter, msg_datetime_format, msg_datetime_format.length, "day");
                text_message_buffer.insert(ref text_iter, "\n", 1);
                last_message_day = sms.date_time.get_day_of_week ();
            }

            string time_message = sms.date_time.format ("%H:%M - ");
            if (sms.from == SMS.FROM_CONTACT) {
                text_message_buffer.insert_with_tags_by_name(ref text_iter, time_message, time_message.length, "time-msg");
                text_message_buffer.insert_with_tags_by_name(ref text_iter, sms.message, sms.message.length, "msg");
            } else {
                text_message_buffer.insert_with_tags_by_name(ref text_iter, time_message, time_message.length, "my-time-msg");
                text_message_buffer.insert_with_tags_by_name (ref text_iter, sms.message, sms.message.length, "my-msg");
            }

            text_message_buffer.insert(ref text_iter, "\n\n", 2);

            if(adding_old_sms == false) {
                text_view.scroll_to_mark (text_mark_auto_scroll, 0, true, 0, 0);
            }
        }

        private void send_message (string message, Device device) {
            SMS sms = new SMS (
                message,
                SMS.FROM_ME,
                SMS.FROM_TYPE_EOSCONNECT,
                new DateTime.now_utc ());

            if (device.has_capability_handler (TelephonyHandler.TELEPHONY) ) {
                ((TelephonyHandler)device.get_path_capability_handler(TelephonyHandler.TELEPHONY))
                    .send_sms (device, sms_contact.phone_number, message);

                SMSStore.instance ().add_sms (device, sms, sms_contact);
            }
        }
    }
}

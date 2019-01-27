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
using MConnect;

namespace Conecto {

    public class DebugToolbar : Gtk.Grid {

        private Views.DevicesView device_view;
        private Gtk.Entry text_data_1;

        public DebugToolbar (Views.DevicesView device_view) {
            this.device_view = device_view;
        }

        construct {
            Gtk.ComboBoxText combo_box_packets = new Gtk.ComboBoxText ();
            combo_box_packets.append_text ("telephony - sms");
            combo_box_packets.append_text ("telephony - sms2");
            combo_box_packets.append_text ("dismiss notification");
            combo_box_packets.set_active(0);
            combo_box_packets.margin = 10;

            text_data_1 = new Gtk.Entry ();
            text_data_1.width_request = 100;
            text_data_1.margin = 10;

            Gtk.Button button_send_packet = new Gtk.Button.with_label ("send packet");
            button_send_packet.margin = 10;

            // left, top, width = 1, height = 1
            attach (combo_box_packets,  0, 0, 1, 1);
            attach (text_data_1,        1, 0, 1, 1);
            attach (button_send_packet, 2, 0, 1, 1);

            button_send_packet.clicked.connect (() => {
                switch (combo_box_packets.get_active_text()) {
                    case "dismiss notification":
                        dismiss_notification_id ();
                        break;
                    case "telephony - sms":
                        build_and_send_telephony_sms_packet ();
                        break;
                    case "telephony - sms2":
                        build_and_send_telephony_sms_packet2 ();
                        break;
                }
            });
        }

        private Device get_selected_device () {
            Widgets.DeviceListBox device_list_box = device_view.device_list_box;
            Widgets.DeviceListBoxRow device_list_box_row = ((Widgets.DeviceListBoxRow)device_list_box.get_selected_row ());

            return device_list_box_row.device;
        }

        private void build_and_send_telephony_sms_packet () {

            string phone_thumbnail;
            try {
                FileUtils.get_contents ("/home/share/Développements/vala/eos-connect/data/test/telephony-thumbnail-source.b64", out phone_thumbnail);
            } catch (Error e) {
                warning ("Unable to load the test phone thumbnail.");
            }

            var body_builder = new Json.Builder ();
            body_builder.begin_object ();
            body_builder.set_member_name ("event");
            body_builder.add_string_value ("sms");
            body_builder.set_member_name ("messageBody");
            body_builder.add_string_value ("It's a test message.");
            body_builder.set_member_name ("contactName");
            body_builder.add_string_value ("John Doe");
            body_builder.set_member_name ("phoneThumbnail");
            body_builder.add_string_value (phone_thumbnail);
            body_builder.set_member_name ("phoneNumber");
            body_builder.add_string_value ("+33102030405");
            body_builder.end_object ();

            var packet = new Packet (TelephonyHandler.TELEPHONY, body_builder.get_root ().get_object (), GLib.Random.next_int ());
            info ("%s", packet.to_string ());

            Device device = get_selected_device ();
            ((TelephonyHandler)device.get_path_capability_handler (TelephonyHandler.TELEPHONY)).message (device, packet);
        }

        private void build_and_send_telephony_sms_packet2 () {

            string phone_thumbnail;
            try {
                FileUtils.get_contents ("/home/share/Développements/vala/eos-connect/data/test/telephony-thumbnail-source2.b64", out phone_thumbnail);
            } catch (Error e) {
                warning ("Unable to load the test phone thumbnail.");
            }

            var body_builder = new Json.Builder ();
            body_builder.begin_object ();
            body_builder.set_member_name ("event");
            body_builder.add_string_value ("sms");
            body_builder.set_member_name ("messageBody");
            body_builder.add_string_value ("Glop message z");
            body_builder.set_member_name ("contactName");
            body_builder.add_string_value ("John Doe");
            body_builder.set_member_name ("phoneThumbnail");
            body_builder.add_string_value (phone_thumbnail);
            body_builder.set_member_name ("phoneNumber");
            body_builder.add_string_value ("+33102030405");
            body_builder.end_object ();

            var packet = new Packet (TelephonyHandler.TELEPHONY, body_builder.get_root ().get_object (), GLib.Random.next_int ());
            info ("%s", packet.to_string ());

            Device device = get_selected_device ();
            ((TelephonyHandler)device.get_path_capability_handler (TelephonyHandler.TELEPHONY)).message (device, packet);
        }

        private void dismiss_notification_id () {
            var notification_id = text_data_1.get_text ();
            Device device = get_selected_device ();

            debug ("Trying to dismiss notification ID %s on device %s", notification_id, device.custom_name);

            var body_builder = new Json.Builder ();
            body_builder.begin_object ();
            body_builder.set_member_name ("cancel");
            body_builder.add_string_value (notification_id);
            body_builder.end_object ();

            var packet = new Packet (
                NotificationHandler.NOTIFICATION_REQUEST,
                body_builder.get_root ().get_object ()
            );

            device.send (packet);
        }
    }
}

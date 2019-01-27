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
 using Conecto.Plugin;
 using Gee;
 using MConnect;

 namespace Conecto.Plugin.Widgets {

     public class SMSTextEntry : Gtk.Box {

         public signal void message_to_send (string text, Device device);
         public HashMap<string, Device> devices_map { get; construct; }

         private Gtk.ComboBoxText combo_box_devices;
         public Gtk.TextView entrytext_view { get; private set; }

         public SMSTextEntry (HashMap<string, Device> devices_map) {
             Object (
                 devices_map: devices_map
             );
         }

         construct {
             combo_box_devices = new Gtk.ComboBoxText ();
             combo_box_devices.margin = 5 ;
             combo_box_devices.valign = Gtk.Align.START ;

             entrytext_view = new Gtk.TextView ();
             entrytext_view.border_width = 1;
             entrytext_view.height_request = 50;
             entrytext_view.hexpand = true;
             entrytext_view.monospace = true;
             entrytext_view.key_press_event.connect ((event_key) => {handle_key_pressed (event_key);});

             var send_message_button = new Gtk.Button.with_label (_("Send message"));
             send_message_button.margin = 5;

             send_message_button.clicked.connect (handle_button_clicked);

             var input_grid = new Gtk.Grid ();
             // left, top, width = 1, height = 1
             input_grid.attach (entrytext_view,       0, 0, 2, 1);
             input_grid.attach (combo_box_devices,    0, 6, 1, 1);
             input_grid.attach (send_message_button,  1, 6, 1, 1);

             add (input_grid);
         }

         public void update_combo_box_devices () {
             combo_box_devices.remove_all ();
             bool active_setup = false;
             foreach (var entry in devices_map.entries) {
                 if (entry.value.is_paired == true && entry.value.is_active == true &&
                     entry.value.has_capability_handler (TelephonyHandler.TELEPHONY) == true &&
                     entry.value.is_capabality_activated (TelephonyHandler.TELEPHONY) == true) {

                     combo_box_devices.append (entry.value.to_unique_string (), entry.value.custom_name);
                     if (active_setup == false) {
                         active_setup = true;
                         combo_box_devices.set_active_id (entry.value.to_unique_string ());
                     }
                 }
             }
         }

         private void handle_button_clicked () {
             send_signal ();
         }

         private bool handle_key_pressed (Gdk.EventKey event_key) {
             if (Gdk.ModifierType.SHIFT_MASK in event_key.state && event_key.keyval == 65293) {
                 send_signal ();
                 return true;
             }

             return false;
         }

         private void send_signal () {
             Device device = devices_map.@get (combo_box_devices.get_active_id ());
             message_to_send (entrytext_view.buffer.text, device);
             entrytext_view.buffer.text = "";
         }
     }
 }

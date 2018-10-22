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

namespace EOSConnect.Views {

    public class DeviceSettingsView : Granite.SimpleSettingsPage {

        public Widgets.DeviceListBox device_list_box { get; construct; }
        public Device device { get; construct; }
        public MainWindow main_window { get; construct; }

        public DeviceSettingsView (Device device, Widgets.DeviceListBox device_list_box, MainWindow main_window) {
            Object (
                activatable: true,
                description: device.name, // always original device name here.
                icon_name: "phone",
                title: "",
                device: device,
                device_list_box: device_list_box,
                main_window: main_window
            );
        }

        construct {

            var custom_name = device.name;
            if (device.custom_name.length > 1) {
                custom_name =  device.settings.get_string ("custom-name");
            }

            title = custom_name;
            icon_name = Tools.get_icon_name (Tools.get_icon_name (device.device_type));

            if(device.is_paired) {
                status_switch.active = true;
            }

            var custom_name_label = new Gtk.Label (_("Custom name"));
            custom_name_label.xalign = 1;

            var custom_name_entry = new Gtk.Entry ();
            custom_name_entry.hexpand = true;
            custom_name_entry.placeholder_text = custom_name;

            var plugin_list_box = new Widgets.PluginListBox (
                device,
                main_window
            );
            plugin_list_box.update_ui (device.plugins_map);

            var scrolled_window = new Gtk.ScrolledWindow (null, null);
            scrolled_window.expand = true;
            scrolled_window.hscrollbar_policy = Gtk.PolicyType.NEVER;

            scrolled_window.add (plugin_list_box);

            content_area.attach (custom_name_label, 0, 1, 1, 1);
            content_area.attach (custom_name_entry, 1, 1, 1, 1);
            content_area.attach (scrolled_window, 0, 4, 4, 3);

            margin = 12;
            show_all ();

            device.settings.bind ("custom-name", custom_name_entry, "text", SettingsBindFlags.DEFAULT);

            status_switch.notify["active"].connect (() => {
                if (status_switch.active) {
                    // ClientIface.get_instance ().allow_device.begin (device.id);
                } else {
                    // ClientIface.get_instance ().disallow_device.begin (device.id);
                }
            });

            custom_name_entry.changed.connect (() => {
               title = custom_name_entry.text;
               device.custom_name = custom_name_entry.text;
               device_list_box.update_selected_list_box_row ();
           });
        }

        public void update_ui () {
            if(device.is_paired) {
                status_switch.active = true;
            }
        }
    }
}

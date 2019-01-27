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
using Granite.Widgets;
using MConnect;

namespace Conecto.Views {

    public class DevicesView : Gtk.Frame {

        public MainWindow main_window { get; construct; }
        public Widgets.DeviceListBox device_list_box { public get; private set; }
        private Gtk.Stack _stack;
        private Widgets.DeviceListActionBarFooter device_list_action_bar_footer;
        private string _latest_id_selected = null;

        public DevicesView (MainWindow main_window) {
            Object (
                main_window: main_window
            );
        }

        construct {
            var scrolled_window = new Gtk.ScrolledWindow (null, null);
            scrolled_window.expand = true;
            scrolled_window.hscrollbar_policy = Gtk.PolicyType.NEVER;

            device_list_box = new Widgets.DeviceListBox ();
            scrolled_window.add (device_list_box);

            device_list_action_bar_footer = new Widgets.DeviceListActionBarFooter (main_window);

            var sidebar_grid = new Gtk.Grid ();
            sidebar_grid.orientation = Gtk.Orientation.VERTICAL;
            sidebar_grid.add (scrolled_window);
            sidebar_grid.add (device_list_action_bar_footer);

            _stack = new Gtk.Stack ();
            _stack.set_transition_type (Gtk.StackTransitionType.CROSSFADE);

            var paned = new Gtk.Paned (Gtk.Orientation.HORIZONTAL);
            paned.pack1 (sidebar_grid, true, false);
            paned.pack2 (_stack, true, false);

            margin = 12;
            add (paned);

            device_list_box.row_selected.connect (_device_list_selected);
        }

        public void update_device_list (HashMap<string, Device> devices_map) {

            foreach (var entry in devices_map.entries) {
                _add_device_settings (entry.value);

                bool device_already_in_list_box = false;
                int count = 0;
                device_list_box.@foreach (() => {
                    Widgets.DeviceListBoxRow device_list_box_row = (Widgets.DeviceListBoxRow)device_list_box.get_row_at_index (count);
                    if (device_list_box_row.device.id == entry.value.id) {
                        device_already_in_list_box = true;
                        // Make sure we keep current selected device like the previous.. selected device.
                        if(device_list_box_row.device.id == _latest_id_selected) {
                            device_list_box.select_row (device_list_box.get_row_at_index (count));
                        }

                        ((Widgets.DeviceListBoxRow)device_list_box.get_row_at_index(count)).update_ui ();
                    }
                    count++;
                });

                if (device_already_in_list_box == false) {
                    device_list_box.insert (new Widgets.DeviceListBoxRow (entry.value), count);
                }
            }

            // First time we populate the list box.
            if (device_list_box.get_selected_row () == null && _latest_id_selected == null) {
                device_list_box.select_row (device_list_box.get_row_at_index (0));
                _latest_id_selected = ((Widgets.DeviceListBoxRow)device_list_box.get_row_at_index (0)).device.id;
            }

            show_all ();
        }

        private void _add_device_settings (Device device) {

            if (_stack.get_child_by_name (device.id) == null) {
                DeviceSettingsView device_settings_view = new DeviceSettingsView (device, device_list_box, main_window);
                device_settings_view.width_request = 150;
                debug ("Adding DeviceSettingsView Widget for device '%s'".printf (device.name));
                _stack.add_named (device_settings_view, device.id);
            } else {
                ((DeviceSettingsView)_stack.get_child_by_name (device.id)).update_ui ();
            }
        }

        private void _device_list_selected (Gtk.ListBoxRow? device_item) {
           Device? device = null;

           if (device_item != null) {
               device = ((Widgets.DeviceListBoxRow)device_item).device;
               _latest_id_selected = device.id;
               _stack.set_visible_child_name (device.id);
               device_list_action_bar_footer.update_ui (device);
           }
       }
    }
}

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
using EOSConnect.Plugin;

namespace EOSConnect.Widgets {

    public class PluginListBox : Gtk.ListBox {

        public weak Device device { get; construct; }
        public MainWindow main_window { get; construct; }
        private Gtk.Label plugins_label;

        public PluginListBox (Device device, MainWindow main_window) {
            Object (
                device: device,
                main_window: main_window
            );
        }

        construct {
            selection_mode = Gtk.SelectionMode.SINGLE;
            set_header_func (update_headers);

            plugins_label = new Gtk.Label (_("Available plugins"));
            plugins_label.get_style_context ().add_class (Granite.STYLE_CLASS_H4_LABEL);
            plugins_label.halign = Gtk.Align.START;
            show_all ();
        }

        public void update_ui (Gee.HashMap<string, PluginInterface> plugins_map) {
            GLib.List<weak Gtk.Widget> pluginlist_items = get_children ();
            foreach (unowned Gtk.Widget plugin_item in pluginlist_items) {
                remove (plugin_item);
            }

            foreach (var entry in plugins_map.entries) {

                PluginListBoxRow plugin_list_box_row = entry.value.get_plugin_list_box_row (entry.value, device);
                plugin_list_box_row.update_ui ();

                entry.value.parent_window = main_window;
                entry.value.row_configure_button = plugin_list_box_row.row_configure_button;

                insert (plugin_list_box_row, 0);
            }

            show_all ();
        }

        private void update_headers (Gtk.ListBoxRow row, Gtk.ListBoxRow? before = null) {
            if(before == null) {
                row.set_header (plugins_label);
            }
        }
    }
}

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

namespace EOSConnect.Widgets {

    public class DeviceListBox : Gtk.ListBox {

        private Gtk.Label _devices_label;

        construct {
            selection_mode = Gtk.SelectionMode.SINGLE;
            set_header_func (update_headers);

            _devices_label = new Gtk.Label (_("Devices"));
            _devices_label.get_style_context ().add_class (Granite.STYLE_CLASS_H4_LABEL);
            _devices_label.halign = Gtk.Align.START;
            show_all ();
        }

        public void update_selected_list_box_row () {

            var list_box_row = (DeviceListBoxRow)this.get_selected_row ();
            list_box_row.update_ui ();
        }

        private void update_headers (Gtk.ListBoxRow row, Gtk.ListBoxRow? before = null) {
            if(before == null) {
                row.set_header (_devices_label);
            }
        }
    }
}

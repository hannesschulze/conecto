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
using MConnect;

namespace Conecto.Plugin.Widgets {

    public class SMSContactListBox : Gtk.ListBox {

        private Gtk.Label contacts_label;

        construct {

            selection_mode = Gtk.SelectionMode.SINGLE;
            set_header_func (update_headers);

            contacts_label = new Gtk.Label (_("Contacts"));
            contacts_label.get_style_context ().add_class (Granite.STYLE_CLASS_H4_LABEL);
            contacts_label.halign = Gtk.Align.START;
            show_all ();

            // this.row_activated.connect ( (list_box_row) => {
            //     warning("par la");
            // });
        }

        public void update_selected_list_box_row () {

            var list_box_row = (SMSContactListBoxRow)this.get_selected_row ();
            list_box_row.update_ui ();
        }

        private void update_headers (Gtk.ListBoxRow row, Gtk.ListBoxRow? before = null) {
            if(before == null) {
                row.set_header (contacts_label);
            }
        }
    }
}

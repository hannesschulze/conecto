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

namespace Conecto.Plugin.Popover {

    public class ShareSettings : Gtk.Popover {

        public Device device { get; construct; }

        public ShareSettings (Device device, Gtk.Widget? relative_to) {
            Object (
               device: device,
               relative_to: relative_to,
               modal: true,
               position: Gtk.PositionType.BOTTOM
           );
        }

        construct {

            var share_icon = new Gtk.Image.from_icon_name ("emblem-shared", Gtk.IconSize.DND);
            share_icon.pixel_size = 64;
            share_icon.margin = 5;

            var desc_label = new Granite.HeaderLabel (_("Download directory"));

            var file_chooser = new Gtk.FileChooserButton (_("Select default download directory"),
                                                          Gtk.FileChooserAction.SELECT_FOLDER);
            var directory = device.settings.get_string ("kdeconnect-share-directory");
            if (directory.length > 5) {
                file_chooser.set_uri (directory);
            } else {
                file_chooser.set_uri (ShareHandler.DOWNLOADS);
            }



            // left, top, width, height
            var grid = new Gtk.Grid ();
            grid.attach (share_icon,     0, 0, 1, 3);
            grid.attach (desc_label,     1, 0, 3, 1);
            grid.attach (file_chooser,   1, 1, 5, 1);

            add (grid);

    		file_chooser.selection_changed.connect (() => {
    			device.settings.set_string ("kdeconnect-share-directory", file_chooser.get_uri ());
    		});

        }
    }
}

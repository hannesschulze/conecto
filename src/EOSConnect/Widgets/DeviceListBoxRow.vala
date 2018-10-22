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

    public class DeviceListBoxRow : Gtk.ListBoxRow {

        public weak Device device { get; construct; }

        private Gtk.Image status_image;
        private Gtk.Label _device_description;
        private Gtk.Label _device_name;
        private Gtk.Image _device_icon;

        public DeviceListBoxRow (Device device) {
            Object (device: device);
        }

        construct {

            var overlay = new Gtk.Overlay ();
            overlay.width_request = 38;

            var grid = new Gtk.Grid ();
            grid.margin = 6;
            grid.column_spacing = 6;

            _device_icon = new Gtk.Image.from_icon_name (Tools.get_icon_name (device.device_type), Gtk.IconSize.DND);
            _device_icon.pixel_size = 32;

            _device_name = new Gtk.Label (device.custom_name);
            _device_name.get_style_context ().add_class (Granite.STYLE_CLASS_H3_LABEL);
            _device_name.ellipsize = Pango.EllipsizeMode.END;
            _device_name.halign = Gtk.Align.START;
            _device_name.valign = Gtk.Align.START;

            _device_description = new Gtk.Label (Tools.get_status_text (device));
            _device_description.margin_top = 2;
            _device_description.use_markup = true;
            _device_description.ellipsize = Pango.EllipsizeMode.END;
            _device_description.halign = Gtk.Align.START;
            _device_description.valign = Gtk.Align.START;

            var hbox = new Gtk.Box (Gtk.Orientation.HORIZONTAL, 0);
            hbox.pack_start (_device_description, false, false, 0);

            status_image = new Gtk.Image.from_icon_name (Tools.get_status_icon_name (device), Gtk.IconSize.MENU);
            status_image.halign = status_image.valign = Gtk.Align.END;

            overlay.add (_device_icon);
            overlay.add_overlay (status_image);

            grid.attach (overlay, 0, 0, 1, 2);
            grid.attach (_device_name, 1, 0, 1, 1);
            grid.attach (hbox, 1, 1, 1, 1);

            add (grid);
        }

        public void update_ui () {
            _device_name.label = device.custom_name;
            _device_description.label = Tools.get_status_text (device);
            status_image.icon_name = Tools.get_status_icon_name (device);
        }
    }
}

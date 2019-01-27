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

namespace Conecto.Widgets {

    public class DeviceListActionBarFooter : Gtk.ActionBar {

        public MainWindow main_window { get; construct; }

        private Gtk.Button button_remove;

        public DeviceListActionBarFooter (MainWindow main_window) {
            Object (main_window: main_window);
        }

        construct {
            button_remove = new Gtk.Button.from_icon_name ("list-remove-symbolic", Gtk.IconSize.SMALL_TOOLBAR);
            button_remove.sensitive = false;
            button_remove.tooltip_text = _("Forget selected device");

            var refresh_devices_button = new Gtk.Button.from_icon_name ("view-refresh");
            refresh_devices_button.tooltip_text = _("Refresh devices");

            get_style_context ().add_class (Gtk.STYLE_CLASS_INLINE_TOOLBAR);
            add (refresh_devices_button);
            add (button_remove);

            button_remove.clicked.connect (() => {
                debug ("@TODO - remove device.");
            });

            refresh_devices_button.clicked.connect (() => {
                // main_window.display_overlaybar (_("Searching for devices..."));
                // ClientIface.get_instance ().get_device_list.begin ();
            });
        }

        public void update_ui (Device device) {
            button_remove.sensitive = false;
            // if (device.is_paired == false && device.is_connected == false) {
            //     button_remove.sensitive = true;
            // }
        }
    }
}

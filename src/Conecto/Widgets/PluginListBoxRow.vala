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
using Conecto.Plugin;
using MConnect;

namespace Conecto.Widgets {
    public class PluginListBoxRow : Gtk.ListBoxRow {

        public weak PluginInterface plugin_interface { get; construct; }
        public weak Device device { get; construct; }

        //protected Gtk.Overlay overlay;
        protected Gtk.Grid row_grid;
        public Gtk.Switch row_switch;
        protected Gtk.Label row_description;
        protected Gtk.Label row_name;
        protected Gtk.Image row_image;
        public Gtk.Button row_configure_button { public get; private set; }

        public PluginListBoxRow (
                PluginInterface plugin_interface,
                Device device
        ) {
            Object (
                plugin_interface: plugin_interface,
                device: device
            );
        }

        construct {

            row_grid = new Gtk.Grid ();
            row_grid.margin = 6;
            row_grid.column_spacing = 12;

            row_image = new Gtk.Image.from_icon_name (plugin_interface.icon_name, Gtk.IconSize.DND);
            row_image.pixel_size = 32;
            row_image.sensitive = plugin_interface.is_active;

            row_name = new Gtk.Label (plugin_interface.name);
            row_name.xalign = 0;
            row_name.get_style_context ().add_class (Granite.STYLE_CLASS_H3_LABEL);
            row_name.sensitive = plugin_interface.is_active;

            row_description = new Gtk.Label (plugin_interface.description);
            row_description.ellipsize = Pango.EllipsizeMode.END;
            row_description.hexpand = true;
            row_description.xalign = 0;
            row_description.sensitive = plugin_interface.is_active;

            row_switch = new Gtk.Switch ();
            row_switch.tooltip_text = _("Activate / Deactivate this plugin");
            row_switch.valign = Gtk.Align.CENTER;
            row_switch.active = plugin_interface.is_active;

            row_configure_button = new Gtk.Button.from_icon_name ("open-menu-symbolic");
            row_configure_button.tooltip_text = _("Configure this plugin");
            row_configure_button.valign = Gtk.Align.CENTER;
            row_configure_button.sensitive = plugin_interface.is_active;

            device.settings.bind (plugin_interface.settings_key_is_active, row_switch, "active", SettingsBindFlags.DEFAULT);

            if(plugin_interface.show_configure_button == true) {
                row_configure_button.clicked.connect ( () => { plugin_interface.handle_configure_button (device); });
            }

            row_switch.notify["active"].connect (() => {
                device.plugins_map.@get (plugin_interface.capability_name).is_active =  row_switch.active;
                if(plugin_interface.show_configure_button == true) {
                    row_configure_button.sensitive = row_switch.active;
                    row_description.sensitive = row_switch.active;
                    row_name.sensitive = row_switch.active;
                    row_image.sensitive = row_switch.active;
                }
            });
        }

        public virtual void update_ui () {

            row_grid.attach (row_image, 0, 0, 1, 2);
            row_grid.attach (row_name, 1, 0, 1, 1);
            row_grid.attach (row_description, 1, 1, 1, 1);

            if(plugin_interface.show_configure_button == true) {
                row_grid.attach (row_configure_button, 2, 0, 1, 2);
                row_grid.attach (row_switch, 3, 0, 1, 2);
            } else {
                row_grid.attach (row_switch, 2, 0, 1, 2);
            }

            add (row_grid);
        }
    }
}

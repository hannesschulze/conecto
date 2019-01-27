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
using Conecto.Widgets;
using MConnect;

namespace Conecto.Plugin {

    public abstract class PluginInterface : GLib.Object {

        public string name  { get; protected set; }
        public string icon_name  { get; protected set; default = "info"; }
        public string capability_name  { get; protected set; }
        public string description { get; protected set; }
        public bool is_active = true;
        public bool show_configure_button = false;
        public string settings_key_is_active { get; protected set; }
        public GLib.Application? application { protected get; public set; }
        public Gtk.Window? parent_window { protected get; public set; }
        public Gtk.Button? row_configure_button { protected get; public set; }

        public virtual void init () {
            info ("Plugin %s do not have configuration window.", name);
        }

        public virtual PluginListBoxRow get_plugin_list_box_row (PluginInterface plugin, Device device) {
            return new PluginListBoxRow (plugin, device);
        }

        public virtual void handle_configure_button (Device device) {
            info ("Plugin %s do not have configuration window.", name);
        }
    }
}

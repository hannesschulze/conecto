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
using EOSConnect.Widgets;
using EOSConnect.Plugin;
using MConnect;

namespace EOSConnect.Plugin {

    public class Ping : PluginInterface {

        construct {
            name  = _("Ping");
            icon_name = "network-transmit-receive";
            capability_name = PingHandler.PING;
            description = _("Send or receive a ping.");
            settings_key_is_active = "kdeconnect-ping-active";
        }

        public override PluginListBoxRow get_plugin_list_box_row (PluginInterface plugin, Device device) {
            return new Widgets.PingPluginListBoxRow (plugin, device);
        }
    }
}

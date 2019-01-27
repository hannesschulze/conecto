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
using EOSConnect.Plugin.Popover;
using EOSConnect.Widgets;
using Gee;
using MConnect;

namespace EOSConnect.Plugin {

    public class Battery : PluginInterface {

        public bool battery_low_level_notify_is_active = true;
        public double battery_low_level_notify_treshold = 5;
        private BatterySettings _battery_settings_popover;

        construct {
            name  = _("Battery");
            icon_name = "battery";
            capability_name = BatteryHandler.BATTERY;
            description = _("Receive battery information");
            settings_key_is_active = "kdeconnect-battery-active";
            show_configure_button = true;
        }

        public override PluginListBoxRow get_plugin_list_box_row (PluginInterface plugin, Device device) {
            return new Widgets.BatteryPluginListBoxRow (plugin, device);
        }

        public override void handle_configure_button (Device device) {
            if (_battery_settings_popover == null) {
                var _battery_settings_popover = new BatterySettings (device, row_configure_button);
                _battery_settings_popover.show_all ();

                _battery_settings_popover.destroy.connect (() => {
                    _battery_settings_popover = null;
                });
            }
        }
    }
}

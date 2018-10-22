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
using Gee;
using EOSConnect;
using EOSConnect.Widgets;
using EOSConnect.Plugin;
using EOSConnect.Plugin.Popover;
using MConnect;

namespace EOSConnect.Plugin {

    public class Telephony : PluginInterface {

        private TelephonySettings _telephony_settings_popover;

        construct {
            name  = _("Telephony");
            icon_name = "phone";
            capability_name = TelephonyHandler.TELEPHONY;
            description = _("Calls notification and SMS.");
            settings_key_is_active = "kdeconnect-telephony-active";
            show_configure_button = true;
        }

        public override PluginListBoxRow get_plugin_list_box_row (PluginInterface plugin, Device device) {
            return new Widgets.TelephonyPluginListBoxRow (plugin, device);
        }

        public override void handle_configure_button (Device device) {
            if (_telephony_settings_popover == null) {
                var _telephony_settings_popover = new TelephonySettings (device, row_configure_button);
                _telephony_settings_popover.show_all ();

                _telephony_settings_popover.destroy.connect (() => {
                    _telephony_settings_popover = null;
                });
            }
        }
    }
}

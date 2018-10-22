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
 * Maciek Borzecki <maciek.borzecki (at] gmail.com>
 * gyan000 <gyan000 (at] ijaz.fr>
 */
using EOSConnect;

namespace MConnect {

    public class BatteryHandler : Object, PacketHandlerInterface {

        public signal void battery_update ();

        public const string BATTERY = "kdeconnect.battery";

        public string get_pkt_type () {
            return BATTERY;
        }

        public BatteryHandler () { }

        public static BatteryHandler instance () {
            return new BatteryHandler ();
        }

        public void use_device (Device device) {
            debug ("Use device %s for battery status updates.", device.to_string ());
            device.message.connect (message);
        }

        public void release_device (Device device) {
            debug ("Release device %s", device.to_string ());
            device.message.disconnect (message);
        }

        public void message (Device device, Packet pkt) {

            if (pkt.pkt_type != BATTERY) {
                return;
            }

            debug ("Got battery packet.");

            int64 level = pkt.body.get_int_member ("currentCharge");
            bool charging = pkt.body.get_boolean_member ("isCharging");

            debug ("Battery level: %u %s", (uint)level, (charging == true) ? "charging" : "");
            device.battery_level = (double)level;
            device.battery_charging = charging;
            battery_update ();

            Plugin.Battery plugin_battery = (Plugin.Battery)device.get_plugin (BATTERY);
            if (plugin_battery.battery_low_level_notify_is_active == false) {
                return;
            }

            if (device.battery_level <= plugin_battery.battery_low_level_notify_treshold) {

                string notif_summary = "Battery warning \u0040" + device.custom_name;
                string notif_body =  "<i>Charge at " + device.battery_level.to_string () + "%</i>\n";

                var notification = new GLib.Notification (notif_summary);
                    notification.set_body (notif_body);
                    notification.set_icon (new GLib.ThemedIcon (Utils.get_icon_name (device.device_type)));
                try {
                    Core.instance ().application.send_notification (device.id + "_battery_notif", notification);
                } catch (Error e) {
                    warning ("Error: %s", e.message);
                }
            }
        }
    }
}

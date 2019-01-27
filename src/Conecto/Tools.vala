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

namespace EOSConnect {

    public class Tools {

        public static string get_icon_name(string device_type) {
            string icon_name = device_type;
            if (device_type == "tablet") {
                icon_name = "computer-apple-ipad";
            }

            return icon_name;
        }

        public static string get_status_icon_name (Device device) {
            string status_icon_name = "user-offline";

            if (device.is_paired == true && device.is_active == true)
                status_icon_name = "user-available";
            else if (device.is_paired == true && device.is_active == false)
                status_icon_name = "user-away";

            return status_icon_name;
        }

        public static string get_status_text (Device device) {
            string status_text = _("Not paired");

            if (device.is_paired == true && device.is_active == true)
                status_text = _("Connected");
            else if (device.is_paired == true && device.is_active == false)
                status_text = _("Not connected");
            else if (device.is_paired == false && device.is_active == false)
                status_text = _("Not paired and not connected");

            return status_text;
        }

        public static string get_battery_icon_name (Device device) {
            string battery_icon_name = "battery";

            if (device.battery_level == 100 && device.battery_charging == true) {
                battery_icon_name = "battery-full-charged";
            } else if (device.battery_level >= 99) {
                battery_icon_name = (device.battery_charging == true) ? "battery-full-charging" : "battery-full";
            } else if (device.battery_level >= 60 && device.battery_level < 99) {
                battery_icon_name = (device.battery_charging == true) ? "battery-good-charging" : "battery-good";
            } else if (device.battery_level >= 10 && device.battery_level < 60) {
                battery_icon_name = (device.battery_charging == true) ? "battery-low-charging" : "battery-low";
            } else if  (device.battery_level >= 2 && device.battery_level < 10) {
                battery_icon_name = (device.battery_charging == true) ? "battery-caution-charging" : "battery-caution";
            } else {
                battery_icon_name = (device.battery_charging == true) ? "battery-empty-charging" : "battery-empty";
            }

            return battery_icon_name;
        }
    }
}

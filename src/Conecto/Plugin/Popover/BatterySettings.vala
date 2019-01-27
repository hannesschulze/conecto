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
using EOSConnect;
using MConnect;

namespace EOSConnect.Plugin.Popover {

    public class BatterySettings : Gtk.Popover {

        public Device device { get; construct; }

        public BatterySettings (Device device, Gtk.Widget? relative_to) {
            Object (
               device: device,
               relative_to: relative_to,
               modal: true,
               position: Gtk.PositionType.BOTTOM
           );
        }

        construct {


            var battery_icon = new Gtk.Image.from_icon_name ("battery-caution", Gtk.IconSize.DND);
            battery_icon.pixel_size = 64;
            battery_icon.margin = 5;
            battery_icon.sensitive = device.settings.get_boolean ("kdeconnect-battery-notify-low-level-active");

            var desc_label = new Granite.HeaderLabel (_("Notify when battery charge level is low"));

            var charge_label = new Gtk.Label (_("Charge percentage"));
            charge_label.halign = Gtk.Align.START;
            charge_label.margin = 10;
            charge_label.sensitive = device.settings.get_boolean ("kdeconnect-battery-notify-low-level-active");

            var charge_battery_spin = new Gtk.SpinButton.with_range (
                (double)1, (double)99, (double)1);
            charge_battery_spin.halign = Gtk.Align.CENTER;
            charge_battery_spin.hexpand = false;
            charge_battery_spin.margin = 10;
            charge_battery_spin.value = device.settings.get_double("kdeconnect-battery-low-level-treshold");

            device.settings.bind ("kdeconnect-battery-low-level-treshold", charge_battery_spin,
                "value", SettingsBindFlags.DEFAULT);

            var activation_switch = new Gtk.Switch ();
            activation_switch.tooltip_text = _("Activate / Deactivate battery alert");
            activation_switch.halign = Gtk.Align.END;
            activation_switch.hexpand = false;
            activation_switch.margin = 10;

            device.settings.bind ("kdeconnect-battery-notify-low-level-active", activation_switch,
                "active", SettingsBindFlags.DEFAULT);

            charge_battery_spin.sensitive = device.settings.get_boolean ("kdeconnect-battery-notify-low-level-active");

            // left, top, width, height1
            var grid = new Gtk.Grid ();
            grid.attach (battery_icon,        0, 0, 1, 3);
            grid.attach (desc_label,          1, 0, 3, 1);
            grid.attach (charge_label,        1, 1, 1, 1);
            grid.attach (charge_battery_spin, 2, 1, 1, 1);
            grid.attach (activation_switch,   3, 1, 1, 1);

            add (grid);

            activation_switch.notify["active"].connect( () => {
                ((Plugin.Battery)device.plugins_map.@get (BatteryHandler.BATTERY))
                    .battery_low_level_notify_is_active = activation_switch.active;
                battery_icon.sensitive = activation_switch.active;
                charge_label.sensitive = activation_switch.active;
                charge_battery_spin.sensitive = activation_switch.active;
            });

            charge_battery_spin.notify["value"].connect( () => {
                ((Plugin.Battery)device.plugins_map.@get (BatteryHandler.BATTERY))
                    .battery_low_level_notify_treshold = charge_battery_spin.value;
            });
        }
    }
}

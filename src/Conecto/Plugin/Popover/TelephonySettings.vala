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
using Conecto;
using MConnect;

namespace Conecto.Plugin.Popover {

    public class TelephonySettings : Gtk.Popover {

        public Device device { get; construct; }

        public TelephonySettings (Device device, Gtk.Widget? relative_to) {
            Object (
               device: device,
               relative_to: relative_to,
               modal: true,
               position: Gtk.PositionType.BOTTOM
           );
        }

        construct {

            var history_icon = new Gtk.Image.from_icon_name ("mail-mark-read", Gtk.IconSize.DND);
            history_icon.pixel_size = 64;
            history_icon.margin = 5;
            //history_icon.sensitive = device.settings.get_boolean ("kdeconnect-battery-notify-low-level-active");

            var desc_label = new Granite.HeaderLabel (_("Store SMS"));

            var history_day_label = new Gtk.Label (_("Number of days of history"));
            history_day_label.halign = Gtk.Align.START;
            history_day_label.margin = 10;
            //history_day_label.sensitive = device.settings.get_boolean ("kdeconnect-battery-notify-low-level-active");

            var history_day_spin = new Gtk.SpinButton.with_range (
                (double)1, (double)99, (double)1);
            history_day_spin.halign = Gtk.Align.CENTER;
            history_day_spin.hexpand = false;
            history_day_spin.margin = 10;
            //history_day_spin.value = device.settings.get_double("kdeconnect-battery-low-level-treshold");


            // device.settings.bind ("kdeconnect-battery-low-level-treshold", history_day_spin,
            //     "value", SettingsBindFlags.DEFAULT);

            var activation_switch = new Gtk.Switch ();
            activation_switch.tooltip_text = _("Activate / Deactivate history");
            activation_switch.halign = Gtk.Align.END;
            activation_switch.hexpand = false;
            activation_switch.margin = 10;

            // device.settings.bind ("kdeconnect-battery-notify-low-level-active", activation_switch,
            //     "active", SettingsBindFlags.DEFAULT);

            // history_day_spin.sensitive = device.settings.get_boolean ("kdeconnect-battery-notify-low-level-active");

            // left, top, width, height1
            var grid = new Gtk.Grid ();
            grid.attach (history_icon,        0, 0, 1, 3);
            grid.attach (desc_label,          1, 0, 3, 1);
            grid.attach (history_day_label,        1, 1, 1, 1);
            grid.attach (history_day_spin, 2, 1, 1, 1);
            grid.attach (activation_switch,   3, 1, 1, 1);

            add (grid);

            activation_switch.notify["active"].connect( () => {
                // ((Plugin.Battery)device.plugins_map.@get (BatteryHandler.BATTERY))
                //     .battery_low_level_notify_is_active = activation_switch.active;
                history_icon.sensitive = activation_switch.active;
                history_day_label.sensitive = activation_switch.active;
                history_day_spin.sensitive = activation_switch.active;
            });

            // history_day_spin.notify["value"].connect( () => {
            //     ((Plugin.Battery)device.plugins_map.@get (BatteryHandler.BATTERY))
            //         .battery_low_level_notify_treshold = history_day_spin.value;
            // });
        }
    }
}

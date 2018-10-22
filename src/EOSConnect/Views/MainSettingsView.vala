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
namespace EOSConnect.Views {

    public class MainSettingsView : Gtk.Stack {

        public MainWindow main_window { get; construct; }
        public GLib.Settings main_settings { get; construct; }

        public MainSettingsView (MainWindow main_window, GLib.Settings main_settings) {
            Object (
                main_settings: main_settings,
                main_window: main_window
            );
        }

        construct {
            var sms_store_row = new OptionListBoxRow (
                new OptionImage("mail-mark-read"),
                new OptionLabelTitle (_("Store SMS")),
                new OptionLabelDescription (_("Active / Deactivate the storage of SMS for all devices.")),
                new OptionSwitch (_("Active / Deactivate the storage of SMS for all devices."),
                                  "kdeconnect-telephony-store-sms-globally-active", _main_settings));

            var list_box = new Gtk.ListBox ();
            list_box.insert (sms_store_row, 0);

            var scrolled_window = new Gtk.ScrolledWindow (null, null);
            scrolled_window.expand = true;
            scrolled_window.hscrollbar_policy = Gtk.PolicyType.NEVER;
            scrolled_window.add (list_box);

            margin = 12;
            add(scrolled_window);
        }

        private class OptionImage : Gtk.Image {
            public OptionImage (string icon_name) {
                set_from_icon_name (icon_name, Gtk.IconSize.DND);
                pixel_size = 32;
            }
        }

        private class OptionLabelTitle : Gtk.Label {
            public OptionLabelTitle (string text) {
                label = text;
                xalign = 0;
                ellipsize = Pango.EllipsizeMode.END;
                get_style_context ().add_class (Granite.STYLE_CLASS_H3_LABEL);
            }
        }

        private class OptionLabelDescription: Gtk.Label {
            public OptionLabelDescription (string text) {
                label = text;
                ellipsize = Pango.EllipsizeMode.END;
                hexpand = true;
                xalign = 0;
            }
        }

        private class OptionSwitch: Gtk.Switch {
            public OptionSwitch (string tooltip, string settings_key, GLib.Settings main_settings) {
                tooltip_text = tooltip;
                valign = Gtk.Align.CENTER;
                active = main_settings.get_boolean (settings_key);
                main_settings.bind (settings_key, this, "active", SettingsBindFlags.DEFAULT);
            }
        }

        private class OptionListBoxRow : Gtk.ListBoxRow {
            public OptionListBoxRow (
                    OptionImage image,
                    OptionLabelTitle label_title,
                    OptionLabelDescription label_description,
                    OptionSwitch switch
                ) {

                var grid = new Gtk.Grid ();
                grid.margin = 6;
                grid.column_spacing = 12;
                grid.attach (image, 0, 0, 1, 2);
                grid.attach (label_title, 1, 0, 1, 1);
                grid.attach (label_description, 1, 1, 1, 1);
                grid.attach (@switch, 2, 0, 1, 2);

                add (grid);
            }
        }
    }
}

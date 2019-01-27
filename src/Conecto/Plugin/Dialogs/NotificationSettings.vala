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

namespace Conecto.Plugin.Dialogs {

    public class NotificationSettings : Gtk.Dialog {

        public Device device { get; construct; }
        private Gtk.ListBox listbox_applications;
        private Gtk.Entry filter_listbox_application_entry;

        public NotificationSettings (Device device, Gtk.Window? window_parent) {
            Object (
               border_width: 5,
               deletable: false,
               resizable: false,
               title: _("Notifications settings"),
               transient_for: window_parent,
               device: device
           );
        }

        construct {

            listbox_applications = new Gtk.ListBox ();

            Gtk.ScrolledWindow scrolled_window = new Gtk.ScrolledWindow (null, null);
            scrolled_window.expand = true;
            scrolled_window.hscrollbar_policy = Gtk.PolicyType.NEVER;
            scrolled_window.add (listbox_applications);

            filter_listbox_application_entry = new Gtk.Entry ();
            filter_listbox_application_entry.hexpand = true;

            var general_grid = new Gtk.Grid ();
            general_grid.column_spacing = 12;
            general_grid.row_spacing = 6;
            general_grid.attach (new Granite.HeaderLabel (_("General")), 0, 0, 2, 1);
            general_grid.attach (new SettingsLabel (_("Receive nofitications")), 0, 1, 1, 1);
            general_grid.attach (new SettingsSwitch ("kdeconnect-notifications-receive", this.device), 1, 1, 1, 1);
            general_grid.attach (new SettingsLabel (_("Send nofitications")), 0, 2, 1, 1);
            general_grid.attach (new SettingsSwitch ("kdeconnect-notifications-send", this.device), 1, 2, 1, 1);

            general_grid.attach (new Granite.HeaderLabel (_("Applications")), 0, 3, 2, 1);
            general_grid.attach (new SettingsLabel (_("Filter in applications")), 0, 5, 1, 1);
            general_grid.attach (filter_listbox_application_entry, 1, 5, 3, 1);
            general_grid.attach (scrolled_window, 0, 6, 4, 50);

            get_content_area ().add (general_grid);

            var close_button = new Gtk.Button.with_label (_("Close"));
                close_button.clicked.connect (() => {
                listbox_applications = null;
                destroy ();
            });

            add_action_widget (close_button, 0);

            listbox_applications.set_filter_func (filter_listbox_applications);
            Gtk.IconTheme theme = Gtk.IconTheme.get_default ();
            foreach (unowned AppInfo appinfo in AppInfo.get_all ()) {
                if ("-device.desktop" in appinfo.get_id ()) {
                    continue;
                }

                listbox_applications.insert (
                new SettingsListBoxRow (appinfo, theme,
                    device.settings.get_strv ("kdeconnect-notifications-send-banned"), device), 0);
            }

            filter_listbox_application_entry.changed.connect (() => {
                listbox_applications.invalidate_filter ();
            });
        }

        public bool filter_listbox_applications (Gtk.ListBoxRow row) {

            if(filter_listbox_application_entry.text.length == 0) {
              return true;
            }

            if (filter_listbox_application_entry.text.down () in ((SettingsListBoxRow)row).row_name.label.down ()) {
                return true;
            }

            return false;
        }

        private class SettingsLabel : Gtk.Label {
            public SettingsLabel (string text) {
                label = text;
                halign = Gtk.Align.END;
                margin_start = 12;
            }
        }

        private class SettingsSwitch : Gtk.Switch {
            public SettingsSwitch (string capability_name, Device device) {
                halign = Gtk.Align.START;
                valign = Gtk.Align.CENTER;
                active = device.settings.get_boolean (capability_name);
                device.settings.bind (capability_name, this, "active", SettingsBindFlags.DEFAULT);
                this.notify["active"].connect (() => {
                    if(capability_name == "kdeconnect-notifications-receive") {
                        ((Plugin.Notification)device.plugins_map.@get (NotificationHandler.NOTIFICATION))
                            .settings_receive_is_active =  this.active;
                    }
                    else {
                        ((Plugin.Notification)device.plugins_map.@get (NotificationHandler.NOTIFICATION))
                            .settings_send_is_active =  this.active;
                    }
                });
            }
        }

        private class SettingsListBoxRow : Gtk.ListBoxRow {
            public Gtk.Label row_name { get; private set; }

            public SettingsListBoxRow (
                AppInfo appinfo,
                Gtk.IconTheme theme,
                string[] apps_banned,
                Device device) {

                row_name = new Gtk.Label (appinfo.get_name ());
                Icon? icon = appinfo.get_icon ();

                if (icon != null && icon is ThemedIcon) {
                    Gtk.IconInfo? iconinfo = theme.lookup_by_gicon (icon, 48, 0);
                    if (iconinfo != null) {

                        Gtk.Image row_image = new Gtk.Image.from_gicon (icon, Gtk.IconSize.DND);
                        row_image.pixel_size = 32;

                        Gtk.Grid row_grid = new Gtk.Grid ();
                        row_grid.margin = 6;
                        row_grid.column_spacing = 12;

                        row_name.xalign = 0;
                        row_name.get_style_context ().add_class (Granite.STYLE_CLASS_H3_LABEL);
                        row_name.ellipsize = Pango.EllipsizeMode.END;
                        row_name.hexpand = true;
                        row_name.xalign = 0;

                        Gtk.Switch row_switch = new Gtk.Switch ();
                        row_switch.tooltip_text = _("Activate / Deactivate this plugin");
                        row_switch.valign = Gtk.Align.CENTER;
                        if (appinfo.get_id () in apps_banned) {
                            row_switch.active = false;
                        } else {
                            row_switch.active = true;
                        }

                        row_switch.notify["active"].connect (() => {
                            Plugin.Notification plugin_notification =
                                (Plugin.Notification)device.plugins_map.@get (NotificationHandler.NOTIFICATION);

                            string[] temp_apps_banned = device.settings.get_strv ("kdeconnect-notifications-send-banned");
                            // Dissallow this app to send notification.
                            if (row_switch.active == false) {
                                temp_apps_banned += appinfo.get_id ();
                                plugin_notification.settings_apps_ids_banned.add (appinfo.get_id ());
                            } else { // Allow this app to send notification.
                                // @TOCHECK Probably a more efficient way to do that.
                                Gee.ArrayList<string> new_apps_banned = new Gee.ArrayList<string>.wrap (temp_apps_banned);
                                if (new_apps_banned.contains (appinfo.get_id ())) {
                                    new_apps_banned.remove (appinfo.get_id ());
                                    plugin_notification.settings_apps_ids_banned.remove (appinfo.get_id ());
                                }
                                temp_apps_banned = {};
                                foreach (string app_banned in new_apps_banned) {
                                    temp_apps_banned += app_banned;
                                }
                                new_apps_banned = null;
                            }

                            device.settings.set_strv ("kdeconnect-notifications-send-banned", temp_apps_banned);
                        });

                        // int left, int top, int width = 1, int height = 1
                        row_grid.attach (row_image,  0, 0, 1, 2);
                        row_grid.attach (row_name,   1, 0, 1, 1);
                        row_grid.attach (row_switch, 3, 0, 1, 2);

                        add (row_grid);
                    }
                }
            }
        }
    }
}

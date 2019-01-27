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
using Conecto.Plugin.Dialogs;
using Gee;
using MConnect;

namespace Conecto.Plugin {

    public class @Notification : PluginInterface {

        public bool settings_receive_is_active = true;
        public bool settings_send_is_active = true;
        public ArrayList<string> settings_apps_ids_banned;
        private NotificationSettings notification_settings_dialog;

        construct {
            name  = _("Notification");
            icon_name = "dialog-information";
            capability_name = NotificationHandler.NOTIFICATION;
            description = _("Setup notification to send or received.");
            settings_key_is_active = "kdeconnect-notification-active";
            show_configure_button = true;
        }

        public override void handle_configure_button (Device device) {
            if (notification_settings_dialog == null) {
                notification_settings_dialog = new Plugin.Dialogs.NotificationSettings(device, parent_window);
                notification_settings_dialog.show_all ();

                notification_settings_dialog.destroy.connect (() => {
                    notification_settings_dialog = null;
                });
            }

            notification_settings_dialog.present ();
        }
    }
}

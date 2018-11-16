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
using EOSConnect.Plugin;
using EOSConnect.Plugin.Windows;

namespace EOSConnect.Widgets {

    public class DeviceMenuitem : Dbusmenu.Menuitem {

        private const int SUB_MENU_BATTERY_ID = 100;
        private const int SUB_MENU_TELEPHONY_ID = 200;
        private const int SUB_MENU_SHARE_ID = 300;

        private Device device;
        private MainWindow main_window;
        private Dbusmenu.Menuitem sub_menu_item_battery;
        private Dbusmenu.Menuitem sub_menu_item_telephony;
        private Dbusmenu.Menuitem sub_menu_item_share;

        construct { }

        public DeviceMenuitem.with_device (Device device, MainWindow main_window) {
            Object (
                id: device.device_num
            );

            this.device = device;
            this.main_window = main_window;
            property_set (Dbusmenu.MENUITEM_PROP_LABEL, device.custom_name);
            update_sub_menu_battery ();
            update_sub_menu_telephony ();
            update_sub_menu_share ();
        }

        public void update_ui () {
            update_sub_menu_battery ();
            update_sub_menu_telephony ();
            update_sub_menu_share ();
        }

        private void update_sub_menu_battery () {
            if (device.settings.get_boolean ("kdeconnect-battery-active") == true) {

                if (sub_menu_item_battery == null) {
                    sub_menu_item_battery = new Dbusmenu.Menuitem.with_id (id + SUB_MENU_BATTERY_ID);
                    child_append (sub_menu_item_battery);
                }

                sub_menu_item_battery.property_set (Dbusmenu.MENUITEM_PROP_ICON_NAME,
                    Tools.get_battery_icon_name (device));
                sub_menu_item_battery.property_set (Dbusmenu.MENUITEM_PROP_LABEL,
                    device.battery_level.to_string () + "%");

            } else {
                if (sub_menu_item_battery != null) {
                    child_delete (sub_menu_item_battery);
                    sub_menu_item_battery = null;
                }
            }
        }

        private void update_sub_menu_telephony () {
            if ( device.settings.get_boolean ("kdeconnect-telephony-active") == true) {

                if (sub_menu_item_telephony == null) {
                    sub_menu_item_telephony = new Dbusmenu.Menuitem.with_id (id + SUB_MENU_TELEPHONY_ID);
                    sub_menu_item_telephony.item_activated.connect (() => {
                        main_window.main_stack.set_visible_child_name ("sms_history_view");
                    });
                    child_append (sub_menu_item_telephony);
                }

                sub_menu_item_telephony.property_set (Dbusmenu.MENUITEM_PROP_ICON_NAME, "mail-send");
                sub_menu_item_telephony.property_set (Dbusmenu.MENUITEM_PROP_LABEL, _("Send or view SMS"));
            } else {
                if (sub_menu_item_telephony != null) {
                    child_delete (sub_menu_item_telephony);
                    sub_menu_item_telephony = null;
                }
            }
        }

        private void update_sub_menu_share () {
            if (device.settings.get_boolean ("kdeconnect-share-active") == true) {

                if (sub_menu_item_share == null) {
                    sub_menu_item_share = new Dbusmenu.Menuitem.with_id (id + SUB_MENU_SHARE_ID);
                    sub_menu_item_share.item_activated.connect (() => {
                        ((Share)device.get_plugin (ShareHandler.SHARE_PKT)).send_files (device);
                    });
                    child_append (sub_menu_item_share);
                }

                sub_menu_item_share.property_set (Dbusmenu.MENUITEM_PROP_ICON_NAME, "send-to");
                sub_menu_item_share.property_set (Dbusmenu.MENUITEM_PROP_LABEL, _("Send file(s)"));
            } else {
                if (sub_menu_item_share != null) {
                    child_delete (sub_menu_item_share);
                    sub_menu_item_share = null;
                }
            }
        }


    }
}

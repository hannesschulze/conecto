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
namespace Conecto.Plugin {

    public class SMSStore : GLib.Object {

        private static SMSStoreInterface? sms_store_interface_instance = null;

        public static SMSStoreInterface instance (GLib.Settings ?main_settings = null) {

            if (sms_store_interface_instance == null) {
                sms_store_interface_instance = SMSStoreFactory.get_sms_store (main_settings);
            }
            return sms_store_interface_instance;
        }
    }
}

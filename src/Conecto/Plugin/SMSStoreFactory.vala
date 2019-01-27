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
namespace EOSConnect.Plugin {

    public abstract class SMSStoreFactory : Object {

        public static SMSStoreInterface get_sms_store (GLib.Settings ?main_settings) {

            if (main_settings == null) {
                warning ("Something is wrong here, for the instantiation, we should have a GLib.Settings, will use SMSStoreTemp by default.");
                return new SMSStoreTemp ();
            }

            if (main_settings.get_boolean ("kdeconnect-telephony-store-sms-globally-active") == true) {
                info ("Using SMSStoreSQLite.");
                SMSStoreSQLite sms_sqlite = new SMSStoreSQLite ();
                sms_sqlite.main_settings = main_settings;
                sms_sqlite.init_store ();

                if (sms_sqlite.is_initialized == true &&
                    main_settings.get_boolean ("kdeconnect-telephony-db-setup-done") == false) {
                    if (sms_sqlite.initial_setup () == true) {
                        main_settings.set_boolean ("kdeconnect-telephony-db-setup-done", true);
                    }
                }

                return sms_sqlite;
            }

            info ("Using SMSStoreTemp.");
            SMSStoreTemp sms_temp = new SMSStoreTemp ();
            sms_temp.main_settings = main_settings;

            return sms_temp;
        }
    }
}

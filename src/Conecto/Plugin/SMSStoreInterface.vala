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
using Gee;
using MConnect;

namespace EOSConnect.Plugin {

    public abstract class SMSStoreInterface : Object {

        public signal void sms_handled (SMS sms);
        public signal void contacts_updated (HashMap<string, SMSContact> sms_contact_map);
        public signal void contact_updated (SMSContact sms_contact);

        public bool is_initialized { public get; protected set; default = true; }
        public GLib.Settings main_settings;
        public HashMap<string, SMSContact> sms_contact_map { protected set; get; }
        public HashMap<string, SMS> sms_map { protected set; get; }

        public SMSStoreInterface () {
            sms_contact_map = new HashMap<string, SMSContact> ();
            sms_map = new HashMap<string, SMS> ();
        }

        public abstract void add_sms (Device device, SMS sms, SMSContact sms_contact);
        public virtual bool initial_setup () {
            debug ("This store does not provide an initial_setup method.");
            return true;
        }
        public virtual void init_store () {
            debug ("This store does not provide an init_store method.");
        }
        public virtual Gee.ArrayList<SMS> get_latest_sms_message (SMSContact sms_contact) {
            debug ("This store does not provide a get_latest_sms_message method.");
            return new Gee.ArrayList<SMS> ();
        }
        public virtual Gee.ArrayList<SMS> get_previous_sms_message (SMSContact sms_contact, DateTime before_date_tim) {
            debug ("This store does not provide aget_latest_sms_message method.");
            return new Gee.ArrayList<SMS> ();
        }
        public virtual void handle_missed_sms (Device device, SMS sms, string contact_name) {
            debug ("This store does not provide a handle_missed_sms method.");
        }
        public virtual void refresh_contact_list () {
            debug ("This store does not provide a refresh_contact_list method.");
        }

        public virtual void merge_contact (Contact contact) {
            debug ("This store does not provide a merge_contact method.");
        }
    }
}

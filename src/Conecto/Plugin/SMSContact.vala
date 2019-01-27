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

    public class SMSContact : Object {

        public string custom_name;
        public string contact_image_path;
        public string phone_thumbnail;

        public string contact_name { get; private set; default = ""; }
        public string phone_number { get; private set; default = ""; }
        public string device_id { get; private set; default = ""; }

        public SMSContact (string phone_number,
                           string contact_name,
                           string device_id,
                           string phone_thumbnail,
                           string custom_name) {
           this.phone_number = phone_number;
           this.contact_name = contact_name;
           this.device_id = device_id;
           this.phone_thumbnail = phone_thumbnail;
           this.custom_name = custom_name;
           this.contact_image_path = "";
       }

        public bool set_contact_image () {
            string file_pathname = "/tmp/eos-connect-sms-contact-" + phone_number;
            try {
                if (FileUtils.test (file_pathname, FileTest.IS_REGULAR)) {
                    debug ("Contact image cached: %s", file_pathname);
                    contact_image_path = file_pathname;

                    return true;
                }

                if (phone_thumbnail.length > 10) {

                    FileUtils.set_data (file_pathname, Base64.decode (phone_thumbnail));
                    contact_image_path = file_pathname;
                    phone_thumbnail = null;
                    debug ("Contact image cache generated: %s", contact_image_path);
                }
            } catch (Error e) {
                warning ("Error: %s", e.message);
                contact_image_path = "";
                return false;
            }

            return true;
        }
    }
}

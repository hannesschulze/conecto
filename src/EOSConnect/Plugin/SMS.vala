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

     public class SMS : Object {

         public const int FROM_TYPE_TELEPHONY = 1;
         public const int FROM_TYPE_NOTIFICATION = 2;
         public const int FROM_TYPE_EOSCONNECT = 3;

         public const int FROM_CONTACT = 1;
         public const int FROM_ME = 2;

         public SMSContact? sms_contact;

         public string message { get; private set; default = ""; }
         public int from { get; private set; default = FROM_CONTACT; }
         public int from_type { get; private set; default = FROM_TYPE_TELEPHONY; }
         public DateTime date_time { get; private set; }

         public SMS (string message,
                     int from,
                     int from_type,
                     DateTime date_time) {

            this.message = message;
            this.from = from;
            this.from_type = from_type;
            this.date_time = date_time;
        }
     }
 }

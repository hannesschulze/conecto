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
using Sqlite;

namespace Conecto.Plugin {

    public class SMSStoreSQLite : SMSStoreInterface {

        private const int COL_CONTACT_PHONE = 0;
        private const int COL_CONTACT_THUMBNAIL = 1;
        private const int COL_CONTACT_NAME = 2;
        private const int COL_CONTACT_CUSTOM_NAME = 3;
        private const int COL_CONTACT_DEVICE_ID = 4;

        private const int COL_MESSAGE_TEXT = 1;
        private const int COL_MESSAGE_DATE_RECEIVED = 3;
        private const int COL_MESSAGE_FROM = 4;
        private const int COL_MESSAGE_FROM_TYPE = 5;

        private Database _db;
	    private string _error_msg;
        private int _error_code;

        public override void add_sms (Device device, SMS sms, SMSContact sms_contact) {

            if (sms_contact_map.@has_key (sms_contact.phone_number) == false) {
                debug ("Adding new contact.");
                string query = "INSERT OR IGNORE INTO contact (phone_number, phone_thumbnail, ";
                      query += "name, custom_name, from_device_id) ";
                      query += "VALUES('" + sms_contact.phone_number + "', '" + _res (sms_contact.phone_thumbnail) +  "', ";
                      query += "'" + _res (sms_contact.contact_name) + "', '" + _res (sms_contact.contact_name) + "', ";
                      query += "'" + device.id + "');";

               _exec_query (query);

               sms_contact.set_contact_image ();
               sms_contact_map.@set (sms_contact.phone_number, sms_contact);
               contacts_updated (sms_contact_map);
            }

            debug ("Adding SMS in DB.");
            sms.sms_contact = sms_contact;
            string query = "INSERT INTO message (contact_phone_number, message, from_device_id, ";
                  query += "date_received, `from`, from_type) ";
                  query += "VALUES ('" + sms_contact.phone_number + "', '" + _res (sms.message) + "', ";
                  query += "'" + device.id + "', '" + sms.date_time.to_string () + "', ";
                  query += "'" + sms.from.to_string () + "',  '" + sms.from_type.to_string () + "');";

            _exec_query (query);

            sms.sms_contact = sms_contact;
            sms_handled (sms);
            return;
        }

        public override bool initial_setup () {
            debug ("Creating tables.");
        	string query = """
                CREATE TABLE contact (
                    phone_number	TEXT   PRIMARY KEY NOT NULL,
                    phone_thumbnail	TEXT               NOT NULL,
                    name	        TEXT               NOT NULL,
                    custom_name	    TEXT               NOT NULL,
                    from_device_id  TEXT               NOT NULL
                );

                CREATE TABLE message (
        			contact_phone_number TEXT				    NOT NULL,
                    message     	     TEXT	 	            NOT NULL,
                    from_device_id       TEXT	 	            NOT NULL,
                    date_received        DATETIME               CURRENT_TIMESTAMP,
                    `from`            	 INTEGER                DEFAULT 1,
                    from_type            INTEGER                DEFAULT 1,
                    FOREIGN KEY(contact_phone_number) REFERENCES contact(phone_number)
        		);

                CREATE INDEX idx_message_cpm_m ON message(contact_phone_number,message);
                CREATE INDEX idx_message_cpm ON message(contact_phone_number);
        	""";

            return _exec_query (query);
        }

        public override void init_store () {
            debug ("Sqlite initialization.");
            string db_file = Path.build_filename(Config.create_if_not_exists_config_dir (), "sms-history.db");
            int error_code = Database.open (db_file, out _db);
        	if (error_code != Sqlite.OK) {
        		warning ("Can't open database: %d: %s\n", _db.errcode (), _db.errmsg ());
                is_initialized = false;
        	}
        }

        public override void refresh_contact_list () {
            if (main_settings.get_boolean ("kdeconnect-telephony-db-setup-done") == true) {
                _exec_query ("SELECT * FROM contact;", _refresh_contact_list_callback);
            }
        }

        public override Gee.ArrayList<SMS> get_latest_sms_message (SMSContact sms_contact) {
            var sms_list = new Gee.ArrayList<SMS> ();
            string query = "SELECT * FROM (";
                  query += "SELECT * FROM message WHERE contact_phone_number = '" + sms_contact.phone_number + "' ";
                  query += "ORDER BY date_received DESC LIMIT 20) ";
                  query += "ORDER BY date_received ASC;";
            debug ("SQL: %s", query);
            _exec_query (query, (num_columns, values, column_names) => {
                SMS sms = new SMS (
                    values[COL_MESSAGE_TEXT],
                    int.parse(values[COL_MESSAGE_FROM]),
                    int.parse(values[COL_MESSAGE_FROM_TYPE]),
                    new DateTime.from_iso8601 (values[COL_MESSAGE_DATE_RECEIVED], new TimeZone.utc ()));

                 sms_list.add (sms);
                 return 0;
            });
            return sms_list;
        }

        public override Gee.ArrayList<SMS> get_previous_sms_message (SMSContact sms_contact, DateTime before_date_time) {
            var sms_list = new Gee.ArrayList<SMS> ();
            string query = "SELECT * FROM message WHERE contact_phone_number = '" + sms_contact.phone_number + "' ";
                  query += "AND date_received < '" + before_date_time.to_string () + "' ";
                  query += "ORDER BY date_received DESC LIMIT 10;";
            debug ("SQL: %s", query);
            _exec_query (query, (num_columns, values, column_names) => {
                SMS sms = new SMS (
                    values[COL_MESSAGE_TEXT],
                    int.parse(values[COL_MESSAGE_FROM]),
                    int.parse(values[COL_MESSAGE_FROM_TYPE]),
                    new DateTime.from_iso8601 (values[COL_MESSAGE_DATE_RECEIVED], new TimeZone.utc ()));

                 sms_list.add (sms);
                 return 0;
            });
            return sms_list;
        }

        public override void handle_missed_sms (Device device, SMS sms, string contact_name) {
            int tot = 0;
            string query = "SELECT COUNT(*) AS tot FROM contact WHERE lower(name) = lower('" + _res (contact_name) + "') LIMIT 1";
            debug ("SQL: %s", query);
            _exec_query (query, (num_columns, values, column_names) => {
                tot = values[0].to_int ();
                return 0;
            });

            if (tot == 0) {
                debug ("Unable to find a contact.");
                return;
            }

            query = "SELECT * FROM contact WHERE lower(name) = lower('" + _res (contact_name) + "') LIMIT 1";
            debug ("SQL: %s", query);
            _exec_query (query, (num_columns, values, column_names) => {
                SMSContact sms_contact = new SMSContact (
                    values[COL_CONTACT_PHONE], values[COL_CONTACT_NAME],
                    values[COL_CONTACT_DEVICE_ID], values[COL_CONTACT_THUMBNAIL],
                    values[COL_CONTACT_CUSTOM_NAME]);
                add_sms (device, sms, sms_contact);
                return 0;
            });
        }

        public override void merge_contact (Contact contact) {
            foreach (var entry in sms_contact_map.entries) {
                SMSContact sms_contact = (SMSContact)entry.value;

                if (sms_contact.phone_number == contact.international_phone_number) {
                    debug ("Updating SMSContact detail for %s", sms_contact.phone_number);
                    debug ("\tname    : %s", contact.name);
                    debug ("\tphoto_uri: %s", contact.photo_uri);

                    sms_contact_map.@get (sms_contact.phone_number).contact_image_path =
                        contact.photo_uri.length > 1 ? File.new_for_uri (contact.photo_uri).get_path () : "";
                    sms_contact_map.@get (sms_contact.phone_number).custom_name = contact.name;

                    contact_updated (sms_contact_map.@get (sms_contact.phone_number));
                }
            }
        }

        /**
         * Real escape string - sort of.
         */
        private string _res (string string_to_clean) {
            return string_to_clean.replace ("'", "''").replace ("\"", "\\\"");
        }

        private bool _exec_query (string query, Sqlite.Callback? call_back_function = null) {
            _error_code = _db.exec (query, call_back_function, out _error_msg);
        	if (_error_code != Sqlite.OK) {
        		warning ("Error: %s", _error_msg);
                warning ("Query: %s", query);
                return false;
        	}

            return true;
        }

        private int _refresh_contact_list_callback (int num_columns, string[] values, string[] column_names) {
            debug ("Contact: %s | %s | %s",
                   values[COL_CONTACT_PHONE], values[COL_CONTACT_NAME], values[COL_CONTACT_DEVICE_ID]);

            sms_contact_map.@set (
                values[COL_CONTACT_PHONE],
                new SMSContact (values[COL_CONTACT_PHONE], values[COL_CONTACT_NAME],
                                values[COL_CONTACT_DEVICE_ID], values[COL_CONTACT_THUMBNAIL],
                                values[COL_CONTACT_CUSTOM_NAME]));
            sms_contact_map.@get (values[COL_CONTACT_PHONE]).set_contact_image ();
            contacts_updated (sms_contact_map);

            return 0;
        }
    }
}

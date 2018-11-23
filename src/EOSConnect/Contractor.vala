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

namespace EOSConnect {

    public class Contractor {

        public static string get_contract_dir () {
            return Path.build_filename (Environment.get_home_dir (), ".local", "share", "contractor");
        }

        public static string create_if_not_exists_contract_dir () {
            if (File.new_for_path(Contractor.get_contract_dir ()).query_exists () == false) {
                DirUtils.create (Contractor.get_contract_dir (), 0700);
            }

            return Contractor.get_contract_dir ();
        }

        public async static void clean_contractor_directory () {
            try {
                File directory = File.new_for_path (get_contract_dir ());
                FileEnumerator enumerator = directory.enumerate_children (FileAttribute.STANDARD_NAME, 0);
                FileInfo file_info, iterated_file_info;
                File iterated_file;
                while ((file_info = enumerator.next_file ()) != null) {
                    enumerator.iterate (out iterated_file_info, out iterated_file);
                    debug ("Deleting %s", iterated_file_info.get_name ());
                    iterated_file.delete ();
                }
            } catch (Error e) {
        		warning ("Error: %s\n", e.message);
        	}
        }

        public static void create_contract(Device device) {
            try {
                var contract_file = Path.build_filename (Contractor.create_if_not_exists_contract_dir (), device.id + ".contract");

                debug ("Creating contract : %s", contract_file);
                File file = File.new_for_path (contract_file);
                if (file.query_exists () == true) {
                    file.delete ();
                }


        		FileOutputStream os = file.create (FileCreateFlags.PRIVATE);
                string str_name="Name=Send to " + device.custom_name + "\n";
                string str_desc="Description=Send this file to  " + device.custom_name + "\n";
                // TOCHEK find out why without --print-reply it's not working.
                string str_command  ="Exec=dbus-send --print-reply --dest=com.github.gyan000.eos-connect ";
                       str_command += "/com/github/gyan000/eosconnect/share ";
                       str_command += "com.github.gyan000.eosconnect.Share.Files ";
                       str_command += "string:'" + device.id + "' string:'%F'\n";
        		os.write ("[Contractor Entry]\n".data);
                os.write (str_name.data);
                os.write (str_desc.data);
                os.write ("MimeType=!inode;\n".data);
                os.write (str_command.data);
        	} catch (Error e) {
        		warning ("Error: %s\n", e.message);
        	}
        }

        public static void destroy_contract(Device device) {
            try {
                var contract_file = Path.build_filename (Contractor.create_if_not_exists_contract_dir (), device.id + ".contract");
                File file = File.new_for_path (contract_file);
                if (file.query_exists ()) {
                    debug ("Deleting contract : %s", contract_file);
                    file.delete ();
                }
            } catch (Error e) {
        		warning ("Error: %s\n", e.message);
        	}
        }
    }
}

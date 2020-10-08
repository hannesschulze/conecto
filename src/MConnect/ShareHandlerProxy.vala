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

using Conecto.Plugin;
using Gee;
using MConnect;

[DBus (name = "com.github.hannesschulze.conecto.Share")]
class ShareHandlerProxy : Object {

    // private HashMap<string, Device> devices_map;
    //
    // public ShareHandlerProxy (HashMap<string, Device> devices_map) {
    //      this.devices_map = devices_map;
    // }

    public void files (string device_id, string pathnames) throws GLib.Error {
        try {
            foreach (var entry in Core.instance ().devices_map.entries) {

                if (entry.value.id == device_id &&
                    entry.value.is_active == true) {
                    SList<string> files_to_send = new SList<string> ();

                    debug ("Device ID: %s (%s)", device_id, entry.value.custom_name);
                	foreach (unowned string pathname in pathnames.split (" ")) {
                        files_to_send.append (File.new_for_path (pathname).get_uri ());
                	}

                    ((Share)entry.value.get_plugin (ShareHandler.SHARE_PKT)).send_files (entry.value, files_to_send);
                }
            }
        } catch (Error e) {
            warning ("Error: %s", e.message);
        }
    }
}

[DBus (name = "com.github.hannesschulze.conecto.ShareError")]
public errordomain ShareError {
    SOME_ERROR
}

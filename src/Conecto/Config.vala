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
namespace Conecto {

    public class Config : Object {

        public static string get_config_dir () {
            return Path.build_filename (Environment.get_user_config_dir (), App.APP_NAME);
        }

        public static string create_if_not_exists_config_dir () {
            if (File.new_for_path(Config.get_config_dir ()).query_exists () == false) {
                DirUtils.create (Config.get_config_dir (), 0700);
            }

            return Config.get_config_dir ();
        }
    }
}

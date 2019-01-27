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
 * gyan000 <gyan000(at] ijaz.fr>
 */
 namespace Conecto {
     public class Notification : Object {
        public string id { get; private set; }
        public string app_name { get; private set; }
        public bool is_cancelled { get; private set; }
        public bool is_clearable { get; private set; }
        public string ticker { get; private set; }
        public string title { get; private set; }
        public string text { get; private set; }
        public int64 time { get; private set; }
        public string app_icon { get; private set; }
        public bool app_icon_available { get; private set; }

        public Notification(
            string id,
            string app_name,
            bool is_cancelled,
            bool is_clearable,
            string ticker,
            string title,
            string text,
            int64 time,
            bool app_icon_available,
            string app_icon
        ) {
            this.id = id;
            this.app_name = app_name;
            this.is_cancelled = is_cancelled;
            this.is_clearable = is_clearable;
            this.ticker = ticker;
            this.title = title;
            this.text = text;
            this.time = time;
            this.app_icon_available = app_icon_available;
            this.app_icon = app_icon;
        }
    }
}

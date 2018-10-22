/*-
 * Copyright (c) 2015 Wingpanel Developers (http://launchpad.net/wingpanel)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Library General Public License as published by
 * the Free Software Foundation, either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * gyan000 <gyan000 (at] ijaz.fr>
 *
 * Original code:
 * https://github.com/elementary/wingpanel-indicator-notifications/blob/master/src/Services/Notification.vala
 */

[DBus (name = "org.freedesktop.Notifications")]
public interface DBusNotificationsInterface : Object {
    public signal void notification_closed (uint32 id, uint32 reason);
    public signal void action_invoked (string action, uint32 id);
    public abstract uint32 notify (string app_name,
                                uint32 replaces_id,
                                string app_icon,
                                string summary,
                                string body,
                                string[] actions,
                                HashTable<string, Variant> hints,
                                int32 expire_timeout) throws Error;
}

[DBus (name = "org.freedesktop.DBus")]
public interface DBusInterface : Object {
    [DBus (name = "NameHasOwner")]
    public abstract bool name_has_owner (string name) throws Error;

    [DBus (name = "GetConnectionUnixProcessID")]
    public abstract uint32 get_connection_unix_process_id (string name) throws Error;
}

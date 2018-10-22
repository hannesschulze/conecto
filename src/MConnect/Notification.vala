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
 * gyan000 <gyan000 (at] ijaz.fr
 *
 * Original code:
 * https://github.com/elementary/wingpanel-indicator-notifications/blob/master/src/Services/Notification.vala
 */

public class Notification : Object {

    public const string[] EXCEPTIONS = { "NetworkManager", "gnome-settings-daemon", "gnome-power-panel"};
    public const string DESKTOP_ID_EXT = ".desktop";
    public bool data_session;
    public string app_name;
    public string summary;
    public string message_body;
    public string app_icon;
    public string sender;
    public string[] actions;
    public Variant hints;
    public int32 expire_timeout;
    public uint32 replaces_id;
    public uint32 id;
    public uint32 pid = 0;
    public GLib.DateTime timestamp;
    public int64 unix_time;

    public string desktop_id;
    public AppInfo? app_info = null;

    public signal void closed ();

    private enum Column {
        APP_NAME = 0,
        REPLACES_ID,
        APP_ICON,
        SUMMARY,
        BODY,
        ACTIONS,
        HINTS,
        EXPIRE_TIMEOUT,
        COUNT
    }

    private const string DEFAULT_ACTION = "default";
    private const string X_CANONICAL_PRIVATE_KEY = "x-canonical-private-synchronous";
    private const string DESKTOP_ENTRY_KEY = "desktop-entry";
    private const string FALLBACK_DESKTOP_ID = "gala-other" + DESKTOP_ID_EXT;
    //private bool pid_acquired;

    public Notification.from_message (DBusMessage message, uint32 _id) {
        var body = message.get_body ();

        data_session = false;

        app_name = get_string (body, Column.APP_NAME);
        app_icon = get_string (body, Column.APP_ICON);
        summary = get_string (body, Column.SUMMARY);
        message_body = get_string (body, Column.BODY);
        hints = body.get_child_value (Column.HINTS);
        expire_timeout = get_int32 (body, Column.EXPIRE_TIMEOUT);
        replaces_id = get_uint32 (body, Column.REPLACES_ID);
        id = _id;
        sender = message.get_sender ();

        actions = body.get_child_value (Column.ACTIONS).dup_strv ();
        timestamp = new GLib.DateTime.now_local ();
        unix_time = timestamp.to_unix ();

        desktop_id = lookup_string (hints, DESKTOP_ENTRY_KEY);
        if (desktop_id != "" && !desktop_id.has_suffix (DESKTOP_ID_EXT)) {
            desktop_id += DESKTOP_ID_EXT;

            app_info = new DesktopAppInfo (desktop_id);
        }

        if (app_info == null) {
            desktop_id = FALLBACK_DESKTOP_ID;
            app_info = new DesktopAppInfo (desktop_id);
        }
    }

    public bool get_is_valid () {
        var transient = hints.lookup_value("transient", VariantType.BOOLEAN);
        return app_info != null && hints.lookup_value (X_CANONICAL_PRIVATE_KEY, null) == null && (transient == null || !transient.get_boolean ());
    }


    private string get_string (Variant tuple, int column) {
        var child = tuple.get_child_value (column);
        return child.dup_string ();
    }

    private int32 get_int32 (Variant tuple, int column) {
        var child = tuple.get_child_value (column);
        return child.get_int32 ();
    }

    private uint32 get_uint32 (Variant tuple, int column) {
        var child = tuple.get_child_value (column);
        return child.get_uint32 ();
    }

    private string lookup_string (Variant tuple, string key) {
        var child = tuple.lookup_value (key, null);

        if (child == null || !child.is_of_type (VariantType.STRING)) {
            return "";
        }

        return child.dup_string ();
    }
}

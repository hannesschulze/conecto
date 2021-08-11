/* dock-item-manager.cpp
 *
 * Copyright 2020 Hannes Schulze <haschu0103@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "dock-item-manager.h"
#include "../utils/icons.h"
#include <conecto.h>
#include <plank.h>
#include <glib/gstdio.h>

using namespace App::Controllers;

DockItemManager::DockItemManager ()
    : m_is_listening (false)
{
}

void
DockItemManager::set_models (const Glib::RefPtr<Models::ConnectedDevices>&   connected_devices,
                             const Glib::RefPtr<Models::UnavailableDevices>& unavailable_devices)
{
    m_connected_devices = connected_devices;
    m_unavailable_devices = unavailable_devices;

    m_is_listening = false;
}

void
DockItemManager::listen ()
{
    if (m_is_listening) return;

    schedule_action ([this] () {
        sync ();
        m_connected_devices->signal_row_inserted ().connect (
                sigc::hide (sigc::hide (sigc::mem_fun (*this, &DockItemManager::update_timeout))));
        m_unavailable_devices->signal_row_inserted ().connect (
                sigc::hide (sigc::hide (sigc::mem_fun (*this, &DockItemManager::update_timeout))));
        m_connected_devices->signal_row_deleted ().connect (
                sigc::hide (sigc::mem_fun (*this, &DockItemManager::update_timeout)));
        m_unavailable_devices->signal_row_deleted ().connect (
                sigc::hide (sigc::mem_fun (*this, &DockItemManager::update_timeout)));
        m_connected_devices->signal_row_changed ().connect (
                sigc::hide (sigc::hide (sigc::mem_fun (*this, &DockItemManager::update_timeout))));
        m_unavailable_devices->signal_row_changed ().connect (
                sigc::hide (sigc::hide (sigc::mem_fun (*this, &DockItemManager::update_timeout))));
    });
}

void
DockItemManager::update (std::function<void ()>&& cb)
{
    schedule_action ([this, cb] () {
        sync ();
        cb ();
    });
}

void
DockItemManager::sync ()
{
    PlankDBusClient* client = plank_dbus_client_get_instance ();
    g_assert (plank_dbus_client_get_is_connected (client));

    int     items_len;
    gchar** array = plank_dbus_client_get_persistent_applications (client, &items_len);
    if (!array) return;

    std::string            launcher_location = Conecto::Backend::get_launcher_dir () + "/";
    std::list<std::string> found;
    const std::string      prefix = "file://";

    for (int i = 0; i < items_len; i++) {
        std::string item (array[i]);
        item.erase (0, prefix.size ());
        const std::string filename = item;
        if (!g_str_has_prefix (filename.c_str (), launcher_location.c_str ())) continue;
        if (!g_str_has_suffix (filename.c_str (), ".desktop")) continue;
        item.erase (0, launcher_location.size ());
        item.erase (item.find (".desktop"), item.size ());

        try {
            Glib::KeyFile file;
            file.load_from_file (filename);

            auto dev = find_starred (item);
            if (dev) {
                if (file.get_string (G_KEY_FILE_DESKTOP_GROUP, G_KEY_FILE_DESKTOP_KEY_NAME) != dev->name ||
                    file.get_string (G_KEY_FILE_DESKTOP_GROUP, G_KEY_FILE_DESKTOP_KEY_ICON) != dev->icon_name) {
                    file.set_string (G_KEY_FILE_DESKTOP_GROUP, G_KEY_FILE_DESKTOP_KEY_NAME, dev->name);
                    file.set_string (G_KEY_FILE_DESKTOP_GROUP, G_KEY_FILE_DESKTOP_KEY_ICON, dev->icon_name);
                    file.save_to_file (filename);
                }
                // If found, update the entry
                found.push_back (item);
            } else {
                // If not found, remove the entry
                plank_dbus_client_remove_item (client, array[i]);
                g_remove (filename.c_str ());
                g_debug ("Removed %s from Plank", filename.c_str ());
            }
        } catch (...) {
            plank_dbus_client_remove_item (client, array[i]);
            g_remove (filename.c_str ());
            g_debug ("Removed %s from Plank", filename.c_str ());
        }
    }

    // Add new items
    for (const auto& item : get_starred ()) {
        if (std::find (found.begin (), found.end (), item->id) != found.end ()) continue;
        std::string filename = launcher_location + item->id + ".desktop";

        if (!Gio::File::create_for_path (filename)->query_exists ()) {
            g_debug ("Added %s to Plank", filename.c_str ());
            Glib::KeyFile file;
            file.set_string (G_KEY_FILE_DESKTOP_GROUP, G_KEY_FILE_DESKTOP_KEY_NAME, item->name);
            file.set_string (G_KEY_FILE_DESKTOP_GROUP, G_KEY_FILE_DESKTOP_KEY_COMMENT,
                             "Show the connected device: " + item->name);
            file.set_string (G_KEY_FILE_DESKTOP_GROUP, G_KEY_FILE_DESKTOP_KEY_TYPE, "Application");
            file.set_string (G_KEY_FILE_DESKTOP_GROUP, G_KEY_FILE_DESKTOP_KEY_EXEC,
                             "com.github.hannesschulze.conecto --open-dev " + item->id);
            file.set_string (G_KEY_FILE_DESKTOP_GROUP, G_KEY_FILE_DESKTOP_KEY_ICON, item->icon_name);
            file.set_string (G_KEY_FILE_DESKTOP_GROUP, G_KEY_FILE_DESKTOP_KEY_TERMINAL, "false");
            file.set_string (G_KEY_FILE_DESKTOP_GROUP, G_KEY_FILE_DESKTOP_KEY_ACTIONS, "Reload;CloseAll;CloseAllOther");
            file.save_to_file (filename);
        }
        std::string file_uri = "file://" + filename;
        plank_dbus_client_add_item (client, file_uri.c_str ());
        found.push_back (item->id);
    }
}

std::shared_ptr<DockItemManager::DeviceInfo>
DockItemManager::find_starred (const std::string& id) const
{
    for (const auto& item : m_connected_devices->children ()) {
        if (id == item.get_value (m_connected_devices->column_id) &&
            item.get_value (m_connected_devices->column_starred)) {
            std::string icon_name =
                    Utils::Icons::get_icon_name_for_device_type (item.get_value (m_connected_devices->column_type));
            return std::make_shared<DeviceInfo> (id, item.get_value (m_connected_devices->column_name), icon_name);
        }
    }
    for (const auto& item : m_unavailable_devices->children ()) {
        if (id == item.get_value (m_unavailable_devices->column_id) &&
            item.get_value (m_unavailable_devices->column_starred)) {
            std::string icon_name =
                    Utils::Icons::get_icon_name_for_device_type (item.get_value (m_unavailable_devices->column_type));
            return std::make_shared<DeviceInfo> (id, item.get_value (m_unavailable_devices->column_name), icon_name);
        }
    }
    return std::shared_ptr<DeviceInfo> ();
}

std::list<std::shared_ptr<DockItemManager::DeviceInfo>>
DockItemManager::get_starred () const
{
    std::list<std::shared_ptr<DockItemManager::DeviceInfo>> res;
    for (const auto& item : m_connected_devices->children ()) {
        if (!item.get_value (m_connected_devices->column_starred)) continue;
        std::string icon_name =
                Utils::Icons::get_icon_name_for_device_type (item.get_value (m_connected_devices->column_type));
        res.push_back (std::make_shared<DeviceInfo> (item.get_value (m_connected_devices->column_id),
                                                     item.get_value (m_connected_devices->column_name), icon_name));
    }
    for (const auto& item : m_unavailable_devices->children ()) {
        if (!item.get_value (m_unavailable_devices->column_starred)) continue;
        std::string icon_name =
                Utils::Icons::get_icon_name_for_device_type (item.get_value (m_unavailable_devices->column_type));
        res.push_back (std::make_shared<DeviceInfo> (item.get_value (m_unavailable_devices->column_id),
                                                     item.get_value (m_unavailable_devices->column_name), icon_name));
    }
    return res;
}

void
DockItemManager::update_timeout ()
{
    Glib::signal_timeout ().connect_once (sigc::mem_fun (*this, &DockItemManager::sync), 50);
}

void
DockItemManager::get_position_for_id (const std::string&                                                            id,
                                      std::function<void (int /* x */, int /* y */, Gtk::PositionType /* pos */)>&& cb)
{
    std::string uri = "file://" + Conecto::Backend::get_launcher_dir () + "/" + id + ".desktop";
    schedule_action ([cb, uri] () {
        PlankDBusClient* client = plank_dbus_client_get_instance ();
        gint             x, y;
        GtkPositionType  pos;
        bool             ok = plank_dbus_client_get_hover_position (client, uri.c_str (), &x, &y, &pos);
        if (!ok) return cb (0, 0, Gtk::POS_TOP);

        Gtk::PositionType wrapper_pos;
        switch (pos) {
        case GTK_POS_TOP:
            wrapper_pos = Gtk::POS_TOP;
            break;
        case GTK_POS_LEFT:
            wrapper_pos = Gtk::POS_LEFT;
            break;
        case GTK_POS_RIGHT:
            wrapper_pos = Gtk::POS_RIGHT;
            break;
        default:
            wrapper_pos = Gtk::POS_BOTTOM;
            break;
        }
        cb (x, y, wrapper_pos);
    });
}

void
DockItemManager::schedule_action (const DockAction& action)
{
    PlankDBusClient* client = plank_dbus_client_get_instance ();
    if (plank_dbus_client_get_is_connected (client)) {
        action ();
    } else {
        auto tries = std::make_shared<int> (10);
        Glib::signal_timeout ().connect (
                [this, action, client, tries] () {
                    if (!plank_dbus_client_get_is_connected (client)) return (*tries)-- > 0;
                    action ();
                    return false;
                },
                50);
    }
}
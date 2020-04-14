/* notifications-list.cpp
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

#include "notifications-list.h"

using namespace App::Models;

NotificationsList::NotificationsList (const std::shared_ptr<Conecto::Device>& device)
    : Gtk::ListStore ()
    , m_device (device)
    , m_plugin (std::dynamic_pointer_cast<Conecto::Plugins::Notifications> (
              Conecto::Backend::get_instance ().get_plugin ("kdeconnect.notification")))
{
    m_columns.add (column_app_name);
    m_columns.add (column_id);
    m_columns.add (column_title);
    m_columns.add (column_body);
    m_columns.add (column_time);
    set_column_types (m_columns);

    // Connect to signals
    m_connections.push_back (m_plugin->signal_new_notification ().connect (
            sigc::mem_fun (*this, &NotificationsList::on_new_notification)));
    m_connections.push_back (m_plugin->signal_notification_dismissed ().connect (
            sigc::mem_fun (*this, &NotificationsList::on_notification_dismissed)));
}

Glib::RefPtr<NotificationsList>
NotificationsList::create (const std::shared_ptr<Conecto::Device>& device)
{
    return Glib::RefPtr<NotificationsList> (new NotificationsList (device));
}

void
NotificationsList::on_new_notification (const std::shared_ptr<Conecto::Device>&                  device,
                                        const Conecto::Plugins::Notifications::NotificationInfo& notification)
{
    if (device != m_device) return;

    auto it = append ();
    it->set_value (column_app_name, Glib::ustring (notification.app_name));
    it->set_value (column_id, Glib::ustring (notification.id));
    it->set_value (column_title, Glib::ustring (notification.title));
    it->set_value (column_body, Glib::ustring (notification.body));
    it->set_value (column_time, notification.time.format ("%X"));
}

void
NotificationsList::on_notification_dismissed (const std::shared_ptr<Conecto::Device>& device, const std::string& id)
{
    if (device != m_device) return;

    for (auto& row : children ()) {
        if (row.get_value (column_id) == id) { erase (row); }
    }
}

void
NotificationsList::dismiss (const std::string& id)
{
    // Send a dismiss request via the plugin
    m_plugin->dismiss (m_device, id);
}
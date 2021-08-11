/* notifications-list.h
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

#pragma once

#include <gtkmm.h>
#include <conecto.h>

namespace App {
namespace Models {

/**
 * @brief A model containing a list of notifications for a connected device
 */
class NotificationsList : public Gtk::ListStore {
  public:
    /**
     * Create a new NotificationsList-model for a device using cached data
     */
    static Glib::RefPtr<NotificationsList> create (const std::shared_ptr<Conecto::Device>& device);
    ~NotificationsList ()
    {
        for (auto& conn : m_connections) conn.disconnect ();
    }

    /** @brief The app's name */
    Gtk::TreeModelColumn<Glib::ustring> column_app_name;
    /** @brief The notification's id */
    Gtk::TreeModelColumn<Glib::ustring> column_id;
    /** @brief The title text */
    Gtk::TreeModelColumn<Glib::ustring> column_title;
    /** @brief The main text */
    Gtk::TreeModelColumn<Glib::ustring> column_body;
    /** @brief The time the notification was sent */
    Gtk::TreeModelColumn<Glib::ustring> column_time;

    /** @brief Dismiss a notification, it will be immediately removed from this model */
    void dismiss (const std::string& id);

    NotificationsList (const NotificationsList&) = delete;
    NotificationsList& operator= (const NotificationsList&) = delete;

  private:
    NotificationsList (const std::shared_ptr<Conecto::Device>& device);

    void on_new_notification (const std::shared_ptr<Conecto::Device>&                  device,
                              const Conecto::Plugins::Notifications::NotificationInfo& notification);
    void on_notification_dismissed (const std::shared_ptr<Conecto::Device>& device, const std::string& id);

    /** @brief The device used */
    std::shared_ptr<Conecto::Device>                 m_device;
    std::shared_ptr<Conecto::Plugins::Notifications> m_plugin;
    Gtk::TreeModel::ColumnRecord                     m_columns;
    std::list<sigc::connection>                      m_connections;
};

} // namespace Models
} // namespace App
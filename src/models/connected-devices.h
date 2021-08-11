/* connected-devices.h
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
#include "notifications-list.h"

namespace App {
namespace Models {

/**
 * @brief A model containing a list of paired, online devices
 */
class ConnectedDevices : public Gtk::ListStore {
  public:
    static Glib::RefPtr<ConnectedDevices> create ();
    /**
     * Create a new ConnectedDevices-model using data from the @p Conecto::Backend
     */
    ~ConnectedDevices ()
    {
        for (auto& conn : m_connections) conn.disconnect ();
    }

    /** @brief The device's user-specified name */
    Gtk::TreeModelColumn<Glib::ustring> column_name;
    /** @brief The device's id */
    Gtk::TreeModelColumn<Glib::ustring> column_id;
    /** @brief The device's type */
    Gtk::TreeModelColumn<Glib::ustring> column_type;
    /** @brief The device's battery level [0..100] */
    Gtk::TreeModelColumn<int> column_battery;
    /** @brief true if the device's battery is currently charging */
    Gtk::TreeModelColumn<bool> column_charging;
    /** @brief true if the device is starred */
    Gtk::TreeModelColumn<bool> column_starred;
    /** @brief The host's address */
    Gtk::TreeModelColumn<Glib::ustring> column_host_addr;
    /** @brief The host's port */
    Gtk::TreeModelColumn<uint> column_host_port;
    /** @brief A notifications-model for the device */
    Gtk::TreeModelColumn<Glib::RefPtr<NotificationsList>> column_notifications;

    /**
     * @brief Find a device in the model
     *
     * This may return an invalid iterator
     */
    Gtk::TreeIter find_device (const std::shared_ptr<Conecto::Device>& device) const;
    /**
     * @brief Find a device in the model
     *
     * This may return an invalid iterator
     */
    Gtk::TreeIter find_device (const std::string& id) const;
    /**
     * @brief Get the connected device for a tree iterator
     *
     * If the iterator is invalid, this will return an empty shared_ptr
     */
    std::shared_ptr<Conecto::Device> get_device (const Gtk::TreeIter& iter) const;

    /**
     * @brief Update a device's name
     *
     * @param iter The device iterator
     * @param name The new display name
     */
    void set_device_name (const Gtk::TreeIter& iter, const std::string& name);
    /**
     * @brief Update a starred flag
     *
     * @param iter The device iterator
     * @param name true to make the device starred
     */
    void set_device_starred (const Gtk::TreeIter& iter, bool starred);

    ConnectedDevices (const ConnectedDevices&) = delete;
    ConnectedDevices& operator= (const ConnectedDevices&) = delete;

  private:
    ConnectedDevices ();

    /** @brief The actual device */
    Gtk::TreeModelColumn<std::shared_ptr<Conecto::Device>> column_device;

    void on_new_device (const std::shared_ptr<Conecto::Device>& device);
    void update_for_device (const std::shared_ptr<Conecto::Device>& device);

    Gtk::TreeModel::ColumnRecord               m_columns;
    std::shared_ptr<Conecto::Plugins::Battery> m_battery_plugin;
    std::list<sigc::connection>                m_connections;

    std::map<std::string /* id */, Glib::RefPtr<NotificationsList> /* model */> m_notification_models;
};

} // namespace Models
} // namespace App
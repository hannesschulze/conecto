/* available-devices.h
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
 * @brief A model containing a list of unpaired, online devices
 */
class AvailableDevices : public Gtk::ListStore {
  public:
    static Glib::RefPtr<AvailableDevices> create ();
    /**
     * Create a new AvailableDevices-model using data from the @p Conecto::Backend
     */
    ~AvailableDevices ()
    {
        for (auto& conn : m_connections) conn.disconnect ();
    }

    /** @brief The device's user-specified name */
    Gtk::TreeModelColumn<Glib::ustring> column_name;
    /** @brief The device's id */
    Gtk::TreeModelColumn<Glib::ustring> column_id;
    /** @brief The device's type */
    Gtk::TreeModelColumn<Glib::ustring> column_type;
    /** @brief The host's address */
    Gtk::TreeModelColumn<Glib::ustring> column_host_addr;
    /** @brief The host's port */
    Gtk::TreeModelColumn<uint> column_host_port;
    /** @brief true if the device has sent a pair request */
    Gtk::TreeModelColumn<bool> column_has_requested_pair;
    /** @brief true if we are waiting for a response */
    Gtk::TreeModelColumn<bool> column_pair_in_progress;

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
     * @brief Get the device for a tree iterator
     *
     * If the iterator is invalid, this will return an empty shared_ptr
     */
    std::shared_ptr<Conecto::Device> get_device (const Gtk::TreeIter& iter) const;

    AvailableDevices (const AvailableDevices&) = delete;
    AvailableDevices& operator= (const AvailableDevices&) = delete;

  private:
    AvailableDevices ();

    /** @brief The actual device */
    Gtk::TreeModelColumn<std::shared_ptr<Conecto::Device>> column_device;

    void on_new_device (const std::shared_ptr<Conecto::Device>& device);
    void on_pair_request (const std::shared_ptr<Conecto::Device>& device);
    void update_for_device (const std::shared_ptr<Conecto::Device>& device);

    Gtk::TreeModel::ColumnRecord m_columns;
    std::list<sigc::connection>  m_connections;
};

} // namespace Models
} // namespace App
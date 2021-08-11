/* available-devices.cpp
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

#include "available-devices.h"
#include "../controllers/active-device-manager.h"

using namespace App::Models;

AvailableDevices::AvailableDevices ()
    : Gtk::ListStore ()
{
    m_columns.add (column_device);
    m_columns.add (column_name);
    m_columns.add (column_id);
    m_columns.add (column_type);
    m_columns.add (column_host_addr);
    m_columns.add (column_host_port);
    m_columns.add (column_has_requested_pair);
    m_columns.add (column_pair_in_progress);
    set_column_types (m_columns);

    auto& backend = Conecto::Backend::get_instance ();

    // Register signals devices
    m_connections.push_back (
            backend.signal_found_new_device ().connect (sigc::mem_fun (*this, &AvailableDevices::on_new_device)));
    m_connections.push_back (backend.get_config ().signal_device_changed ().connect (
            sigc::mem_fun (*this, &AvailableDevices::update_for_device)));
}

Glib::RefPtr<AvailableDevices>
AvailableDevices::create ()
{
    return Glib::RefPtr<AvailableDevices> (new AvailableDevices);
}

void
AvailableDevices::on_new_device (const std::shared_ptr<Conecto::Device>& device)
{
    m_connections.push_back (device->signal_connected ().connect (
            sigc::bind (sigc::mem_fun (*this, &AvailableDevices::update_for_device), device)));
    m_connections.push_back (device->signal_disconnected ().connect (
            sigc::bind (sigc::mem_fun (*this, &AvailableDevices::update_for_device), device)));
    m_connections.push_back (device->signal_paired ().connect (
            sigc::hide (sigc::bind<0> (sigc::mem_fun (*this, &AvailableDevices::update_for_device), device))));
    m_connections.push_back (device->signal_pair_request ().connect (
            sigc::bind (sigc::mem_fun (*this, &AvailableDevices::on_pair_request), device)));
    update_for_device (device);
}

void
AvailableDevices::on_pair_request (const std::shared_ptr<Conecto::Device>& device)
{
    update_for_device (device);
    ACTIVE_DEVICE.activate_device (device);
}

void
AvailableDevices::update_for_device (const std::shared_ptr<Conecto::Device>& device)
{
    auto iter = find_device (device);
    bool is_valid = device->get_is_active () && !device->get_is_paired ();

    if (iter && !is_valid) {
        erase (iter);
    } else if (is_valid && !iter) {
        const auto& config = Conecto::Backend::get_instance ().get_config ();
        iter = append ();
        iter->set_value (column_device, device);
        iter->set_value (column_name, config.get_display_name (*device));
        iter->set_value (column_id, Glib::ustring (device->get_device_id ()));
        iter->set_value (column_type, Glib::ustring (device->get_device_type ()));
        iter->set_value (column_host_addr, device->get_host ()->to_string ());
        iter->set_value (column_host_port, device->get_tcp_port ());
        iter->set_value (column_has_requested_pair, device->get_pair_requested ());
        iter->set_value (column_pair_in_progress, device->get_pair_in_progress ());
    } else if (is_valid && iter) {
        const auto& config = Conecto::Backend::get_instance ().get_config ();
        iter->set_value (column_name, config.get_display_name (*device));
        iter->set_value (column_id, Glib::ustring (device->get_device_id ()));
        iter->set_value (column_type, Glib::ustring (device->get_device_type ()));
        iter->set_value (column_host_addr, device->get_host ()->to_string ());
        iter->set_value (column_host_port, device->get_tcp_port ());
        iter->set_value (column_has_requested_pair, device->get_pair_requested ());
        iter->set_value (column_pair_in_progress, device->get_pair_in_progress ());
    }
}

Gtk::TreeIter
AvailableDevices::find_device (const std::shared_ptr<Conecto::Device>& dev) const
{
    const Gtk::TreeNodeChildren& child_nodes = children ();
    for (auto it = child_nodes.begin (); it != child_nodes.end (); it++) {
        if (it->get_value (column_device) == dev) { return it; }
    }
    return Gtk::TreeIter ();
}

Gtk::TreeIter
AvailableDevices::find_device (const std::string& id) const
{
    const Gtk::TreeNodeChildren& child_nodes = children ();
    for (auto it = child_nodes.begin (); it != child_nodes.end (); it++) {
        if (it->get_value (column_id) == id) { return it; }
    }
    return Gtk::TreeIter ();
}

std::shared_ptr<Conecto::Device>
AvailableDevices::get_device (const Gtk::TreeIter& iter) const
{
    if (iter)
        return iter->get_value (column_device);
    else
        return std::shared_ptr<Conecto::Device> ();
}
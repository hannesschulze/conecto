/* unavailable-devices.cpp
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

#include "unavailable-devices.h"

using namespace App::Models;

UnavailableDevices::UnavailableDevices ()
    : Gtk::ListStore ()
{
    m_columns.add (column_device);
    m_columns.add (column_name);
    m_columns.add (column_id);
    m_columns.add (column_type);
    m_columns.add (column_starred);
    set_column_types (m_columns);

    auto& backend = Conecto::Backend::get_instance ();

    // Register signals devices
    m_connections.push_back (
            backend.signal_found_new_device ().connect (sigc::mem_fun (*this, &UnavailableDevices::on_new_device)));
    m_connections.push_back (backend.get_config ().signal_device_changed ().connect (
            sigc::mem_fun (*this, &UnavailableDevices::update_for_device)));
}

Glib::RefPtr<UnavailableDevices>
UnavailableDevices::create ()
{
    return Glib::RefPtr<UnavailableDevices> (new UnavailableDevices);
}

void
UnavailableDevices::on_new_device (const std::shared_ptr<Conecto::Device>& device)
{
    m_connections.push_back (device->signal_connected ().connect (
            sigc::bind (sigc::mem_fun (*this, &UnavailableDevices::update_for_device), device)));
    m_connections.push_back (device->signal_disconnected ().connect (
            sigc::bind (sigc::mem_fun (*this, &UnavailableDevices::update_for_device), device)));
    m_connections.push_back (device->signal_paired ().connect (
            sigc::hide (sigc::bind<0> (sigc::mem_fun (*this, &UnavailableDevices::update_for_device), device))));
    update_for_device (device);
}

void
UnavailableDevices::update_for_device (const std::shared_ptr<Conecto::Device>& device)
{
    auto iter = find_device (device);
    bool is_valid = device->get_is_paired () && !device->get_is_active ();

    if (iter && !is_valid) {
        erase (iter);
    } else if (is_valid && !iter) {
        const auto& config = Conecto::Backend::get_instance ().get_config ();
        iter = append ();
        iter->set_value (column_device, device);
        iter->set_value (column_name, config.get_display_name (*device));
        iter->set_value (column_id, Glib::ustring (device->get_device_id ()));
        iter->set_value (column_type, Glib::ustring (device->get_device_type ()));
        iter->set_value (column_starred, config.get_device_starred (*device));
    } else if (is_valid && iter) {
        const auto& config = Conecto::Backend::get_instance ().get_config ();
        iter->set_value (column_name, config.get_display_name (*device));
        iter->set_value (column_id, Glib::ustring (device->get_device_id ()));
        iter->set_value (column_type, Glib::ustring (device->get_device_type ()));
        iter->set_value (column_starred, config.get_device_starred (*device));
    }
}

Gtk::TreeIter
UnavailableDevices::find_device (const std::shared_ptr<Conecto::Device>& dev) const
{
    const Gtk::TreeNodeChildren& child_nodes = children ();
    for (auto it = child_nodes.begin (); it != child_nodes.end (); it++) {
        if (it->get_value (column_device) == dev) { return it; }
    }
    return Gtk::TreeIter ();
}

Gtk::TreeIter
UnavailableDevices::find_device (const std::string& id) const
{
    const Gtk::TreeNodeChildren& child_nodes = children ();
    for (auto it = child_nodes.begin (); it != child_nodes.end (); it++) {
        if (it->get_value (column_id) == id) { return it; }
    }
    return Gtk::TreeIter ();
}

std::shared_ptr<Conecto::Device>
UnavailableDevices::get_device (const Gtk::TreeIter& iter) const
{
    if (iter)
        return iter->get_value (column_device);
    else
        return std::shared_ptr<Conecto::Device> ();
}

void
UnavailableDevices::set_device_name (const Gtk::TreeIter& iter, const std::string& name)
{
    auto dev = get_device (iter);
    if (dev) Conecto::Backend::get_instance ().get_config ().set_display_name (dev, name);
}

void
UnavailableDevices::set_device_starred (const Gtk::TreeIter& iter, bool starred)
{
    auto dev = get_device (iter);
    if (dev) Conecto::Backend::get_instance ().get_config ().set_device_starred (dev, starred);
}
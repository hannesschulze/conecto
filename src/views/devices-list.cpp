/* devices-list.cpp
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

#include "devices-list.h"

using namespace App::Views;
using namespace App::Models;

DevicesList::DevicesList (const Glib::RefPtr<ConnectedDevices>& connected_devices,
                          const Glib::RefPtr<UnavailableDevices>& unavailable_devices)
    : Gtk::Box (Gtk::ORIENTATION_VERTICAL)
    , m_connected_devices (connected_devices)
    , m_unavailable_devices (unavailable_devices)
    , m_connected_view (connected_devices)
    , m_unavailable_view (unavailable_devices)
{
    m_connected_view.append_column ("Name", m_connected_devices->column_name);
    m_connected_view.append_column ("ID", m_connected_devices->column_id);
    m_connected_view.append_column ("Type", m_connected_devices->column_type);
    m_connected_view.append_column ("Battery", m_connected_devices->column_battery);
    m_connected_view.append_column ("Charging", m_connected_devices->column_charging);
    m_connected_view.append_column ("Starred", m_connected_devices->column_starred);
    m_connected_view.append_column ("Host", m_connected_devices->column_host_addr);
    m_connected_view.append_column ("Port", m_connected_devices->column_host_port);
    pack_start (m_connected_view, true, true);

    m_unavailable_view.append_column ("Name", m_unavailable_devices->column_name);
    m_unavailable_view.append_column ("ID", m_unavailable_devices->column_id);
    m_unavailable_view.append_column ("Type", m_unavailable_devices->column_type);
    m_unavailable_view.append_column ("Starred", m_unavailable_devices->column_starred);
    pack_start (m_unavailable_view, true, true);
}
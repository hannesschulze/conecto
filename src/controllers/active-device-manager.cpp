/* active-device-manager.cpp
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

#include "active-device-manager.h"

using namespace App::Controllers;
using namespace App::Models;

ActiveDeviceManager::ActiveDeviceManager ()
{
}

void
ActiveDeviceManager::set_models (const Glib::RefPtr<Models::ConnectedDevices>& connected_devices,
                                 const Glib::RefPtr<Models::UnavailableDevices>& unavailable_devices,
                                 const Glib::RefPtr<Models::AvailableDevices>& available_devices)
{
    m_connected_devices = connected_devices;
    m_unavailable_devices = unavailable_devices;
    m_available_devices = available_devices;
}

void
ActiveDeviceManager::activate_device (const std::shared_ptr<Conecto::Device>& device)
{
    m_connection_changed.disconnect ();
    m_connection_removed.disconnect ();
    if (device) {
        auto cached = m_device;
        if (m_connected_devices) {
            auto it = m_connected_devices->find_device (device);
            if (it) {
                m_device = device;
                m_signal_connected_device_update.emit (it, true);
                connect_signals (m_connected_devices, m_signal_connected_device_update);
                return;
            }
        }
        if (m_available_devices) {
            auto it = m_available_devices->find_device (device);
            if (it) {
                m_device = device;
                m_signal_available_device_update.emit (it, true);
                connect_signals (m_available_devices, m_signal_connected_device_update);
                return;
            }
        }
        if (m_unavailable_devices) {
            auto it = m_unavailable_devices->find_device (device);
            if (it) {
                m_device = device;
                m_signal_unavailable_device_update.emit (it, true);
                connect_signals (m_unavailable_devices, m_signal_connected_device_update);
                return;
            }
        }
    }
    m_device.reset ();
    m_signal_no_device_selected.emit ();
}
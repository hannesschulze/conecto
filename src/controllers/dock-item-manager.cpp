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
#include <conecto.h>
#include <iostream>
#include <plank.h>

using namespace App::Controllers;

DockItemManager::DockItemManager ()
{
}

void
DockItemManager::set_models (const Glib::RefPtr<Models::ConnectedDevices>& connected_devices,
                             const Glib::RefPtr<Models::UnavailableDevices>& unavailable_devices)
{
    m_connected_devices = connected_devices;
    m_unavailable_devices = unavailable_devices;

    PlankDBusClient* client = plank_dbus_client_get_instance ();
    if (plank_dbus_client_get_is_connected (client)) {
        sync ();
    } else {
        auto tries = std::make_shared<int> (10);
        Glib::signal_timeout ().connect ([this, client, tries]() {
            if (!plank_dbus_client_get_is_connected (client)) {
                return (*tries)-- > 0;
            }
            sync ();
            return false;
        }, 50);
    }
}

void
DockItemManager::sync ()
{
    PlankDBusClient* client = plank_dbus_client_get_instance ();
    g_assert (plank_dbus_client_get_is_connected (client));
    std::cout << "Established a connection" << std::endl;
}
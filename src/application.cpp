/* application.cpp
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

#include "application.h"
#include <conecto.h>
#include "models/connected-devices.h"
#include "models/unavailable-devices.h"
#include "controllers/active-device-manager.h"
#include "controllers/dock-item-manager.h"

using namespace App;

Application::Application ()
    : Gtk::Application (Conecto::Constants::APP_ID)
    , m_connected_devices (Models::ConnectedDevices::create ())
    , m_unavailable_devices (Models::UnavailableDevices::create ())
    , m_available_devices (Models::AvailableDevices::create ())
{
    ACTIVE_DEVICE.set_models (m_connected_devices, m_unavailable_devices, m_available_devices);
    Conecto::Backend::get_instance ().load_from_cache ();
    Conecto::Backend::get_instance ().listen ();

#ifdef ENABLE_PLANK_SUPPORT
    Glib::signal_idle ().connect_once ([this]() {
        DOCK_ITEMS.set_models (m_connected_devices, m_unavailable_devices);
    });
#endif
}

Glib::RefPtr<Application>
Application::create ()
{
    return Glib::RefPtr<Application> (new Application);
}

void
Application::on_activate ()
{
    // Get the current window, create one if it doesn't exist
    Gtk::Window* window = get_active_window ();

    if (!window) {
        m_window = Window::create (m_connected_devices, m_unavailable_devices, m_available_devices);
        add_window (*m_window);
        window = m_window.get ();
    }

    window->present ();
}

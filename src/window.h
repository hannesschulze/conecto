/* window.h
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
#include <memory>
#include "models/connected-devices.h"
#include "models/unavailable-devices.h"
#include "models/available-devices.h"
#include "views/main/devices-list.h"
#include "views/main/active-device-view.h"
#include "widgets/header-bar.h"

namespace App {

/**
 * @brief Conecto's main window
 *
 * Class responsible for creating the main application window (not the dock-popover) that
 * will contain a list of devices as well as other widgets. Because it is a toplevel window,
 * it is managed using a std::shared_ptr
 */
class Window : public Gtk::ApplicationWindow {
  public:
    /**
     * @brief Create a new window and restore its position and size
     *
     * The window should be attached to a @p Gtk::Application using @p Gtk::Application::add_window
     *
     * @param connected_devices The connected-devices model
     * @param unavailable_devices The unavailable-devices model
     * @param available_devices The available-devices model
     * @param sms_storage The SMS-storage model
     */
    static std::shared_ptr<Window> create (const Glib::RefPtr<Models::ConnectedDevices>&   connected_devices,
                                           const Glib::RefPtr<Models::UnavailableDevices>& unavailable_devices,
                                           const Glib::RefPtr<Models::AvailableDevices>&   available_devices,
                                           const std::shared_ptr<Models::SMSStorage>&      sms_storage);
    ~Window () {}

    Window (const Window&) = delete;
    Window& operator= (const Window&) = delete;

  protected:
    Window (const Glib::RefPtr<Models::ConnectedDevices>&   connected_devices,
            const Glib::RefPtr<Models::UnavailableDevices>& unavailable_devices,
            const Glib::RefPtr<Models::AvailableDevices>&   available_devices,
            const std::shared_ptr<Models::SMSStorage>&      sms_storage);

    // Called when the window should be closed, saves the window's position and size
    bool on_delete_event (GdkEventAny* event) override;
    void on_show () override;

  private:
    Gtk::Paned                    m_paned;
    Views::Main::DevicesList      m_devices_list;
    Views::Main::ActiveDeviceView m_active_device;

    std::shared_ptr<Widgets::HeaderBar> m_header_bar;
};

} // namespace App
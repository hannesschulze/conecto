/* device-popover.h
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
#include "widgets/popover-window.h"
#include "widgets/header-bar.h"
#include "views/dock/active-device-view.h"

namespace App {

/**
 * @brief Conecto's device dock-popover
 *
 * Class responsible for creating a popover representing a device opened when clicking on a
 * dock item. Because it is a toplevel window, it is managed using a std::shared_ptr
 */
class DevicePopover : public Widgets::PopoverWindow {
  public:
    /**
     * @brief Create a new device popover and display it at the dock item's position
     *
     * @param app The application this window should be attached to using @p Gtk::Application::add_window
     * @param id The selected device's id
     */
    static std::shared_ptr<DevicePopover> create (Gtk::Application&                             app,
                                                  const Glib::RefPtr<Models::ConnectedDevices>& connected_devices,
                                                  const Glib::ustring&                          id);
    ~DevicePopover () {}

    DevicePopover (const DevicePopover&) = delete;
    DevicePopover& operator= (const DevicePopover&) = delete;

  protected:
    DevicePopover (const Glib::RefPtr<Models::ConnectedDevices>& connected_devices, const Glib::ustring& id);

  private:
    Views::Dock::ActiveDeviceView m_active_view;
};

} // namespace App
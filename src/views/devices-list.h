/* devices-list.h
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
#include "../models/connected-devices.h"

namespace App {
namespace Views {

/**
 * @brief A list shown on the left-hand side of the main window
 * 
 * A list shown in the main window, listing all online, offline and available devices
 * 
 * Connected to the following models: @p App::Models::ConnectedDevices
 */
class DevicesList : public Gtk::TreeView {
  public:
    /**
     * @brief Create a list of devices, visualizing the specified models
     * 
     * @param connected_devices The connected-devices model
     */
    DevicesList (const Glib::RefPtr<Models::ConnectedDevices>& connected_devices);
    ~DevicesList () {}

    DevicesList (const DevicesList&) = delete;
    DevicesList& operator= (const DevicesList&) = delete;

  private:
    Glib::RefPtr<Models::ConnectedDevices> m_connected_devices;
};

} // namespace Views
} // namespace App
/* active-device-view.h
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
#include "../../models/connected-devices.h"
#include "../../models/unavailable-devices.h"
#include "../../models/available-devices.h"
#include "empty-selection-view.h"
#include "available-device-view.h"
#include "unavailable-device-view.h"
#include "connected-device-view.h"

namespace App {
namespace Views {
namespace Main {

/**
 * @brief The main view for the windowed app
 *
 * This view automatically switches between four widgets, depending on the currently activated device:
 * - EmptySelectionView
 * - AvailableDeviceView
 * - UnavailableDeviceView
 * - ConnectedDeviceView
 */
class ActiveDeviceView : public Gtk::Stack {
  public:
    /**
     * @brief Create the widget
     *
     * @param connected_devices The connected-devices model
     * @param unavailable_devices The unavailable-devices model
     * @param available_devices The available-devices model
     */
    ActiveDeviceView (const Glib::RefPtr<Models::ConnectedDevices>&   connected_devices,
                      const Glib::RefPtr<Models::UnavailableDevices>& unavailable_devices,
                      const Glib::RefPtr<Models::AvailableDevices>&   available_devices);
    ~ActiveDeviceView () {}

    /** @brief Get a stack containing all available views for connected devices (typically a @p ConnectedDeviceView) */
    std::shared_ptr<Gtk::Stack> get_connected_device_stack () const;

    ActiveDeviceView (const ActiveDeviceView&) = delete;
    ActiveDeviceView& operator= (const ActiveDeviceView&) = delete;

  private:
    Glib::RefPtr<Models::ConnectedDevices>   m_connected_devices;
    Glib::RefPtr<Models::UnavailableDevices> m_unavailable_devices;
    Glib::RefPtr<Models::AvailableDevices>   m_available_devices;

    void on_update_device (const Gtk::TreeIter& it, bool new_device, const std::string& child_name);
    void on_show_empty ();

    std::shared_ptr<EmptySelectionView>    m_empty_view;
    std::shared_ptr<AvailableDeviceView>   m_available_device_view;
    std::shared_ptr<UnavailableDeviceView> m_unavailable_device_view;
    std::shared_ptr<ConnectedDeviceView>   m_connected_device_view;
};

} // namespace Main
} // namespace Views
} // namespace App
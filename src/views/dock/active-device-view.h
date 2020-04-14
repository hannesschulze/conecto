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
#include "warning-view.h"
#include "connected-device-view.h"

namespace App {
namespace Views {
namespace Dock {

/**
 * @brief The main view for the popover-styled window
 *
 * This view automatically switches between two widgets, depending on the currently activated device:
 * - WarningView
 * - ConnectedDeviceView
 */
class ActiveDeviceView : public Gtk::Stack {
  public:
    /**
     * @brief Create the widget
     *
     * @param connected_devices The connected-devices model
     */
    ActiveDeviceView (const Glib::ustring& id, const Glib::RefPtr<Models::ConnectedDevices>& connected_devices);
    ~ActiveDeviceView () {}

    ActiveDeviceView (const ActiveDeviceView&) = delete;
    ActiveDeviceView& operator= (const ActiveDeviceView&) = delete;

    using type_signal_close_popover = sigc::signal<void>;
    /**
     * Emitted when the popover should be closed (e.g. when the device has been removed)
     */
    type_signal_close_popover signal_close_popover () { return m_signal_close_popover; }

  private:
    Glib::RefPtr<Models::ConnectedDevices> m_connected_devices;

    void on_update_device (const Gtk::TreeIter& it, bool new_device, const std::string& child_name);
    void on_show_empty ();

    std::shared_ptr<WarningView>         m_warning_view;
    std::shared_ptr<ConnectedDeviceView> m_connected_device_view;

    type_signal_close_popover m_signal_close_popover;
};

} // namespace Dock
} // namespace Views
} // namespace App
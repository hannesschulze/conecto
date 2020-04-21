/* active-device-view.cpp
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

#include "active-device-view.h"
#include "../../controllers/active-device-manager.h"

using namespace App::Views::Dock;

ActiveDeviceView::ActiveDeviceView (const Glib::ustring&                          id,
                                    const Glib::RefPtr<Models::ConnectedDevices>& connected_devices,
                                    const std::shared_ptr<Models::SMSStorage>&    sms_storage)
    : Gtk::Stack ()
    , m_warning_view (WarningView::create (id))
    , m_connected_device_view (ConnectedDeviceView::create (connected_devices, sms_storage))
{
    add (*m_warning_view, "warning");
    add (*m_connected_device_view, "connected");
    on_show_empty ();

    m_warning_view->signal_close_popover ().connect (m_signal_close_popover);

    ACTIVE_DEVICE.signal_connected_device_update ().connect (
            sigc::bind (sigc::mem_fun (*this, &ActiveDeviceView::on_update_device), "connected"));
    ACTIVE_DEVICE.signal_unavailable_device_update ().connect (
            sigc::bind (sigc::mem_fun (*this, &ActiveDeviceView::on_update_device), "warning"));
    ACTIVE_DEVICE.signal_available_device_update ().connect (
            sigc::bind (sigc::mem_fun (*this, &ActiveDeviceView::on_update_device), "warning"));
    ACTIVE_DEVICE.signal_no_device_selected ().connect (sigc::mem_fun (*this, &ActiveDeviceView::on_show_empty));
}

void
ActiveDeviceView::on_update_device (const Gtk::TreeIter& it, bool new_device, const std::string& child_name)
{
    if (new_device) set_visible_child (child_name);
}

void
ActiveDeviceView::on_show_empty ()
{
    set_visible_child ("warning");
}
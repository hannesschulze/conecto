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

using namespace App::Views::Main;

ActiveDeviceView::ActiveDeviceView (const Glib::RefPtr<Models::ConnectedDevices>&   connected_devices,
                                    const Glib::RefPtr<Models::UnavailableDevices>& unavailable_devices,
                                    const Glib::RefPtr<Models::AvailableDevices>&   available_devices)
    : Gtk::Stack ()
    , m_connected_devices (connected_devices)
    , m_unavailable_devices (unavailable_devices)
    , m_available_devices (available_devices)
    , m_empty_view (EmptySelectionView::create ())
    , m_available_device_view (AvailableDeviceView::create (available_devices))
    , m_unavailable_device_view (UnavailableDeviceView::create (unavailable_devices))
    , m_connected_device_view (ConnectedDeviceView::create (connected_devices))
{
    add (*m_empty_view, "empty");
    add (*m_connected_device_view, "connected");
    add (*m_unavailable_device_view, "unavailable");
    add (*m_available_device_view, "available");
    on_show_empty ();

    ACTIVE_DEVICE.signal_connected_device_update ().connect (
            sigc::bind (sigc::mem_fun (*this, &ActiveDeviceView::on_update_device), "connected"));
    ACTIVE_DEVICE.signal_unavailable_device_update ().connect (
            sigc::bind (sigc::mem_fun (*this, &ActiveDeviceView::on_update_device), "unavailable"));
    ACTIVE_DEVICE.signal_available_device_update ().connect (
            sigc::bind (sigc::mem_fun (*this, &ActiveDeviceView::on_update_device), "available"));
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
    set_visible_child ("empty");
}

std::shared_ptr<Gtk::Stack>
ActiveDeviceView::get_connected_device_stack () const
{
    return m_connected_device_view;
}
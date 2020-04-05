/* header-bar.cpp
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

#include "header-bar.h"
#include "../controllers/active-device-manager.h"

using namespace App::Widgets;

HeaderBar::HeaderBar (BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder> glade_ref)
    : Gtk::HeaderBar (cobject)
    , m_builder (glade_ref)
{
    ACTIVE_DEVICE.signal_connected_device_update ().connect
        (sigc::mem_fun (*this, &HeaderBar::on_update_connected_device));
    ACTIVE_DEVICE.signal_unavailable_device_update ().connect
        (sigc::mem_fun (*this, &HeaderBar::on_update_unavailable_device));
    ACTIVE_DEVICE.signal_available_device_update ().connect
        (sigc::mem_fun (*this, &HeaderBar::on_update_available_device));
    ACTIVE_DEVICE.signal_no_device_selected ().connect
        (sigc::mem_fun (*this, &HeaderBar::on_reset));
}

std::shared_ptr<HeaderBar>
HeaderBar::create (const Glib::RefPtr<Models::ConnectedDevices>& connected_devices,
                   const Glib::RefPtr<Models::UnavailableDevices>& unavailable_devices,
                   const Glib::RefPtr<Models::AvailableDevices>& available_devices)
{
    HeaderBar* res = nullptr;
    auto builder = Gtk::Builder::create_from_resource ("/com/github/hannesschulze/conecto/ui/widgets/header-bar.ui");
    builder->get_widget_derived ("ConectoWidgetsHeaderBar", res);
    res->m_connected_devices = connected_devices;
    res->m_unavailable_devices = unavailable_devices;
    res->m_available_devices = available_devices;
    res->gobj ();
    return std::shared_ptr<HeaderBar> (res);
}

void
HeaderBar::on_update_connected_device (const Gtk::TreeIter& it, bool new_device)
{
    set_subtitle (it->get_value (m_connected_devices->column_name) + " (Online)");
}

void
HeaderBar::on_update_unavailable_device (const Gtk::TreeIter& it, bool new_device)
{
    set_subtitle (it->get_value (m_unavailable_devices->column_name) + " (Offline)");
}

void
HeaderBar::on_update_available_device (const Gtk::TreeIter& it, bool new_device)
{
    set_subtitle (it->get_value (m_available_devices->column_name));
}

void
HeaderBar::on_reset ()
{
    set_subtitle ("");
}
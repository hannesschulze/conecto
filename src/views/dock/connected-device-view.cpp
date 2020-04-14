/* connected-device-view.cpp
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

#include "connected-device-view.h"
#include "../../controllers/active-device-manager.h"
#include "../../utils/icons.h"

using namespace App::Views::Dock;

ConnectedDeviceView::ConnectedDeviceView (BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder> glade_ref)
    : Gtk::Box (cobject)
    , m_builder (glade_ref)
    , m_grid_sidebar (nullptr)
    , m_lbl_device_name (nullptr)
    , m_lbl_battery_level (nullptr)
    , m_main_stack_switcher (nullptr)
    , m_grid_notifications (nullptr)
{
    m_builder->get_widget ("grid_sidebar", m_grid_sidebar);
    m_builder->get_widget ("lbl_device_name", m_lbl_device_name);
    m_builder->get_widget ("lbl_battery_level", m_lbl_battery_level);
    m_builder->get_widget ("main_stack_switcher", m_main_stack_switcher);
    m_builder->get_widget ("grid_notifications", m_grid_notifications);

    // Add custom widgets
    m_battery_level_widget.set_size_request (96, 96);
    m_battery_level_widget.set_halign (Gtk::ALIGN_CENTER);
    m_battery_level_widget.set_margin_top (36);
    m_grid_sidebar->attach (m_battery_level_widget, 0, 0, 1, 1);

    m_grid_notifications->attach (m_notifications, 0, 0, 1, 1);

    // Update other widgets
    m_main_stack_switcher->set_homogeneous (true);

    // Listen for changes
    ACTIVE_DEVICE.signal_connected_device_update ().connect (sigc::mem_fun (*this, &ConnectedDeviceView::on_update));
}

std::shared_ptr<ConnectedDeviceView>
ConnectedDeviceView::create (const Glib::RefPtr<Models::ConnectedDevices>& connected_devices)
{
    ConnectedDeviceView* res = nullptr;
    auto                 builder = Gtk::Builder::create_from_resource (
            "/com/github/hannesschulze/conecto/ui/views/dock/connected-device-view.ui");
    builder->get_widget_derived ("ConectoViewsDockConnectedDeviceView", res);
    res->m_connected_devices = connected_devices;
    return std::shared_ptr<ConnectedDeviceView> (res);
}

void
ConnectedDeviceView::on_update (const Gtk::TreeIter& iter, bool new_device)
{
    m_battery_level_widget.set_progress (static_cast<double> (iter->get_value (m_connected_devices->column_battery)) /
                                         100.0);
    m_battery_level_widget.set_icon (
            Utils::Icons::get_icon_for_device_type (iter->get_value (m_connected_devices->column_type), 48));
    m_lbl_device_name->set_label (iter->get_value (m_connected_devices->column_name));
    m_lbl_battery_level->set_label (std::to_string (iter->get_value (m_connected_devices->column_battery)) + " %");

    m_notifications.update (iter->get_value (m_connected_devices->column_notifications));
}
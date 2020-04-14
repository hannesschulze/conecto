/* unavailable-device-view.cpp
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

#include "unavailable-device-view.h"
#include "../../controllers/active-device-manager.h"

using namespace App::Views::Main;

UnavailableDeviceView::UnavailableDeviceView (BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder> glade_ref)
    : Gtk::Grid (cobject)
    , m_builder (glade_ref)
    , m_btn_disconnect (nullptr)
    , m_lbl_title (nullptr)
{
    m_builder->get_widget ("btn_disconnect", m_btn_disconnect);
    m_builder->get_widget ("lbl_title", m_lbl_title);

    // Connect to signals
    m_btn_disconnect->signal_clicked ().connect (
            sigc::mem_fun (*this, &UnavailableDeviceView::on_btn_disconnect_clicked));
    ACTIVE_DEVICE.signal_unavailable_device_update ().connect (
            sigc::mem_fun (*this, &UnavailableDeviceView::on_update));
}

std::shared_ptr<UnavailableDeviceView>
UnavailableDeviceView::create (const Glib::RefPtr<Models::UnavailableDevices>& unavailable_devices)
{
    UnavailableDeviceView* res = nullptr;
    auto                   builder = Gtk::Builder::create_from_resource (
            "/com/github/hannesschulze/conecto/ui/views/main/unavailable-device-view.ui");
    builder->get_widget_derived ("ConectoViewsMainUnavailableDeviceView", res);
    res->m_unavailable_devices = unavailable_devices;
    return std::shared_ptr<UnavailableDeviceView> (res);
}

void
UnavailableDeviceView::on_btn_disconnect_clicked ()
{
    auto dev = ACTIVE_DEVICE.get_device ();
    g_assert (dev);
    dev->unpair ();
}

void
UnavailableDeviceView::on_update (const Gtk::TreeIter& iter, bool new_device)
{
    m_lbl_title->set_label (iter->get_value (m_unavailable_devices->column_name) + " is currently unavailable");
}
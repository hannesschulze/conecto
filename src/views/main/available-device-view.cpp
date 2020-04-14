/* available-device-view.cpp
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

#include "available-device-view.h"
#include "../../controllers/active-device-manager.h"

using namespace App::Views::Main;

AvailableDeviceView::AvailableDeviceView (BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder> glade_ref)
    : Gtk::Stack (cobject)
    , m_builder (glade_ref)
    , m_lbl_available (nullptr)
    , m_lbl_request (nullptr)
    , m_btn_accept_request (nullptr)
    , m_btn_reject_request (nullptr)
    , m_btn_send_request (nullptr)
{
    m_builder->get_widget ("lbl_available", m_lbl_available);
    m_builder->get_widget ("lbl_request", m_lbl_request);
    m_builder->get_widget ("btn_accept_request", m_btn_accept_request);
    m_builder->get_widget ("btn_reject_request", m_btn_reject_request);
    m_builder->get_widget ("btn_send_request", m_btn_send_request);

    // Connect to button signals
    m_btn_send_request->signal_clicked ().connect (
            sigc::mem_fun (*this, &AvailableDeviceView::on_send_request_clicked));
    m_btn_accept_request->signal_clicked ().connect (
            sigc::mem_fun (*this, &AvailableDeviceView::on_accept_request_clicked));
    m_btn_reject_request->signal_clicked ().connect (
            sigc::mem_fun (*this, &AvailableDeviceView::on_reject_request_clicked));

    ACTIVE_DEVICE.signal_available_device_update ().connect (sigc::mem_fun (*this, &AvailableDeviceView::on_update));
}

std::shared_ptr<AvailableDeviceView>
AvailableDeviceView::create (const Glib::RefPtr<Models::AvailableDevices>& available_devices)
{
    AvailableDeviceView* res = nullptr;
    auto                 builder = Gtk::Builder::create_from_resource (
            "/com/github/hannesschulze/conecto/ui/views/main/available-device-view.ui");
    builder->get_widget_derived ("ConectoViewsMainAvailableDeviceView", res);
    res->m_available_devices = available_devices;
    return std::shared_ptr<AvailableDeviceView> (res);
}

void
AvailableDeviceView::on_update (const Gtk::TreeIter& iter, bool new_device)
{
    if (iter->get_value (m_available_devices->column_pair_in_progress)) {
        set_visible_child ("pair_in_progress");
    } else if (iter->get_value (m_available_devices->column_has_requested_pair)) {
        m_lbl_request->set_label (iter->get_value (m_available_devices->column_name) +
                                  " has sent a pair request. Do you want to accept it or reject it?");
        set_visible_child ("incoming_request");
    } else {
        m_lbl_available->set_label ("Do you want to pair " + iter->get_value (m_available_devices->column_name) + "?");
        set_visible_child ("send_request");
    }
}

void
AvailableDeviceView::on_send_request_clicked ()
{
    auto dev = ACTIVE_DEVICE.get_device ();
    g_assert (dev);
    dev->pair (true);
}

void
AvailableDeviceView::on_accept_request_clicked ()
{
    auto dev = ACTIVE_DEVICE.get_device ();
    g_assert (dev);
    dev->send_pair_response (true);
}

void
AvailableDeviceView::on_reject_request_clicked ()
{
    auto dev = ACTIVE_DEVICE.get_device ();
    g_assert (dev);
    dev->send_pair_response (false);
}
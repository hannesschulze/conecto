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

namespace {

std::string
get_icon_name_for_starred (bool starred)
{
    return starred ? "starred" : "non-starred";
}

} // namespace

HeaderBar::HeaderBar (BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder> glade_ref)
    : Gtk::HeaderBar (cobject)
    , m_builder (glade_ref)
    , m_device_options_revealer (nullptr)
{
    // Get widgets from Gtk::Builder
    glade_ref->get_widget ("title_stack", m_title_stack);
    glade_ref->get_widget ("lbl_title", m_lbl_title);
    glade_ref->get_widget ("lbl_subtitle", m_lbl_subtitle);
    glade_ref->get_widget ("connected_stack_switcher", m_connected_stack_switcher);
    glade_ref->get_widget ("device_options_revealer", m_device_options_revealer);
    glade_ref->get_widget ("btn_disconnect", m_btn_disconnect);
    glade_ref->get_widget ("btn_toggle_favorite", m_btn_toggle_favorite);
    glade_ref->get_widget ("img_toggle_starred", m_img_toggle_starred);

    // Connect to button events
    m_btn_disconnect->signal_clicked ().connect (sigc::mem_fun (*this, &HeaderBar::on_btn_disconnect_clicked));
    m_btn_toggle_favorite->signal_clicked ().connect (
            sigc::mem_fun (*this, &HeaderBar::on_btn_toggle_favorite_clicked));

    // Connect to active-device changes
    ACTIVE_DEVICE.signal_connected_device_update ().connect (
            sigc::mem_fun (*this, &HeaderBar::on_update_connected_device));
    ACTIVE_DEVICE.signal_unavailable_device_update ().connect (
            sigc::mem_fun (*this, &HeaderBar::on_update_unavailable_device));
    ACTIVE_DEVICE.signal_available_device_update ().connect (
            sigc::mem_fun (*this, &HeaderBar::on_update_available_device));
    ACTIVE_DEVICE.signal_no_device_selected ().connect (sigc::mem_fun (*this, &HeaderBar::on_reset));
}

std::shared_ptr<HeaderBar>
HeaderBar::create (const Glib::RefPtr<Models::ConnectedDevices>&   connected_devices,
                   const Glib::RefPtr<Models::UnavailableDevices>& unavailable_devices,
                   const Glib::RefPtr<Models::AvailableDevices>& available_devices, Gtk::Stack& connected_stack)
{
    HeaderBar* res = nullptr;
    auto builder = Gtk::Builder::create_from_resource ("/com/github/hannesschulze/conecto/ui/widgets/header-bar.ui");
    builder->get_widget_derived ("ConectoWidgetsHeaderBar", res);
    res->m_connected_devices = connected_devices;
    res->m_unavailable_devices = unavailable_devices;
    res->m_available_devices = available_devices;
    res->m_connected_stack_switcher->set_stack (connected_stack);
    res->m_connected_stack_switcher->set_homogeneous (true);
    return std::shared_ptr<HeaderBar> (res);
}

void
HeaderBar::on_update_connected_device (const Gtk::TreeIter& it, bool new_device)
{
    m_title_stack->set_visible_child ("connected");
    m_img_toggle_starred->set_from_icon_name (get_icon_name_for_starred (
                                                      it->get_value (m_connected_devices->column_starred)),
                                              Gtk::ICON_SIZE_LARGE_TOOLBAR);
    m_device_options_revealer->set_reveal_child (true);
}

void
HeaderBar::on_update_unavailable_device (const Gtk::TreeIter& it, bool new_device)
{
    m_title_stack->set_visible_child ("title");
    m_lbl_subtitle->set_label (it->get_value (m_unavailable_devices->column_name) + " (Offline)");
    m_lbl_subtitle->set_visible (true);
    m_img_toggle_starred->set_from_icon_name (get_icon_name_for_starred (
                                                      it->get_value (m_unavailable_devices->column_starred)),
                                              Gtk::ICON_SIZE_LARGE_TOOLBAR);
    m_device_options_revealer->set_reveal_child (true);
}

void
HeaderBar::on_update_available_device (const Gtk::TreeIter& it, bool new_device)
{
    m_title_stack->set_visible_child ("title");
    m_lbl_subtitle->set_label (it->get_value (m_available_devices->column_name));
    m_lbl_subtitle->set_visible (true);
    m_device_options_revealer->set_reveal_child (false);
}

void
HeaderBar::on_reset ()
{
    m_title_stack->set_visible_child ("title");
    m_lbl_subtitle->set_label ("");
    m_lbl_subtitle->set_visible (false);
    m_device_options_revealer->set_reveal_child (false);
}

void
HeaderBar::on_btn_disconnect_clicked ()
{
    auto dev = ACTIVE_DEVICE.get_device ();
    g_assert (dev);
    dev->unpair ();
}

void
HeaderBar::on_btn_toggle_favorite_clicked ()
{
    auto dev = ACTIVE_DEVICE.get_device ();
    g_assert (dev);
    Conecto::ConfigFile& config = Conecto::Backend::get_instance ().get_config ();
    config.set_device_starred (dev, !config.get_device_starred (*dev));
}
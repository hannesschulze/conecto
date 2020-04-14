/* header-bar.h
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
#include "../models/unavailable-devices.h"
#include "../models/available-devices.h"

namespace App {
namespace Widgets {

/**
 * @brief The header bar is responsible for rendering a window decoration for Conecto's main window
 *
 * This view automatically updates the title and disconnect-button, depending on the currently active
 * device
 */
class HeaderBar : public Gtk::HeaderBar {
  public:
    /**
     * @brief Construct the header bar
     *
     * @param connected_devices The connected-devices model
     * @param unavailable_devices The unavailable-devices model
     * @param available_devices The available-devices model
     * @param connected_stack A @p Gtk::Stack containing pages to switch between if a connected device is active
     * (typically a @p ConnectedDeviceView)
     */
    static std::shared_ptr<HeaderBar> create (const Glib::RefPtr<Models::ConnectedDevices>&   connected_devices,
                                              const Glib::RefPtr<Models::UnavailableDevices>& unavailable_devices,
                                              const Glib::RefPtr<Models::AvailableDevices>&   available_devices,
                                              Gtk::Stack&                                     connected_stack);
    ~HeaderBar () {}

    HeaderBar (const HeaderBar&) = delete;
    HeaderBar& operator= (const HeaderBar&) = delete;

    friend Gtk::Builder;

  private:
    HeaderBar (BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder> glade_ref);

    Glib::RefPtr<Models::ConnectedDevices>   m_connected_devices;
    Glib::RefPtr<Models::UnavailableDevices> m_unavailable_devices;
    Glib::RefPtr<Models::AvailableDevices>   m_available_devices;

    Glib::RefPtr<Gtk::Builder> m_builder;

    std::shared_ptr<Conecto::Device> m_active_device;

    // Widgets from the Gtk::Builder
    Gtk::Stack*         m_title_stack;
    Gtk::Label*         m_lbl_title;
    Gtk::Label*         m_lbl_subtitle;
    Gtk::StackSwitcher* m_connected_stack_switcher;
    Gtk::Revealer*      m_device_options_revealer;
    Gtk::Button*        m_btn_disconnect;
    Gtk::Button*        m_btn_toggle_favorite;
    Gtk::Image*         m_img_toggle_starred;

    void on_update_connected_device (const Gtk::TreeIter& it, bool new_device);
    void on_update_unavailable_device (const Gtk::TreeIter& it, bool new_device);
    void on_update_available_device (const Gtk::TreeIter& it, bool new_device);
    void on_reset ();

    void on_btn_disconnect_clicked ();
    void on_btn_toggle_favorite_clicked ();
};

} // namespace Widgets
} // namespace App
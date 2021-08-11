/* unavailable-device-view.h
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
#include "../../models/unavailable-devices.h"

namespace App {
namespace Views {
namespace Main {

/**
 * @brief A view shown if the selected device is paired but currently unavailable
 *
 * This is just a simple screen set up using Glade, providing just the ability to unpair a device
 */
class UnavailableDeviceView : public Gtk::Grid {
  public:
    /**
     * @brief Construct the view
     */
    static std::shared_ptr<UnavailableDeviceView>
    create (const Glib::RefPtr<Models::UnavailableDevices>& unavailable_devices);
    ~UnavailableDeviceView () {}

    UnavailableDeviceView (const UnavailableDeviceView&) = delete;
    UnavailableDeviceView& operator= (const UnavailableDeviceView&) = delete;

    friend Gtk::Builder;

  private:
    UnavailableDeviceView (BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder> glade_ref);

    Glib::RefPtr<Gtk::Builder> m_builder;

    // Widgets from the Gtk::Builder
    Gtk::Button* m_btn_disconnect;
    Gtk::Label*  m_lbl_title;

    Glib::RefPtr<Models::UnavailableDevices> m_unavailable_devices;

    void on_btn_disconnect_clicked ();
    void on_update (const Gtk::TreeIter& iter, bool new_device);
};

} // namespace Main
} // namespace Views
} // namespace App
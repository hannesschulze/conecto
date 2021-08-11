/* available-device-view.h
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
#include "../../models/available-devices.h"

namespace App {
namespace Views {
namespace Main {

/**
 * @brief A view offering the user the option to pair a device
 *
 * This screen consists of three pages: A "send a pair request" screen, a "accept pair request?" screen
 * and a screen used if pairing is in progress
 */
class AvailableDeviceView : public Gtk::Stack {
  public:
    /**
     * @brief Construct the view
     */
    static std::shared_ptr<AvailableDeviceView>
    create (const Glib::RefPtr<Models::AvailableDevices>& available_devices);
    ~AvailableDeviceView () {}

    AvailableDeviceView (const AvailableDeviceView&) = delete;
    AvailableDeviceView& operator= (const AvailableDeviceView&) = delete;

    friend Gtk::Builder;

  private:
    AvailableDeviceView (BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder> glade_ref);

    Glib::RefPtr<Gtk::Builder> m_builder;

    // Widgets from the Gtk::Builder
    Gtk::Label*  m_lbl_available;
    Gtk::Label*  m_lbl_request;
    Gtk::Button* m_btn_accept_request;
    Gtk::Button* m_btn_reject_request;
    Gtk::Button* m_btn_send_request;

    Glib::RefPtr<Models::AvailableDevices> m_available_devices;

    void on_update (const Gtk::TreeIter& iter, bool new_device);
    void on_send_request_clicked ();
    void on_accept_request_clicked ();
    void on_reject_request_clicked ();
};

} // namespace Main
} // namespace Views
} // namespace App
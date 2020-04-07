/* connected-device-view.h
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

namespace App {
namespace Views {

/**
 * @brief A view shown if the active device is currently paired and available
 * 
 * This screen is made up of three sections: Notifications and device info, SMS and File Transfer
 */
class ConnectedDeviceView : public Gtk::Stack {
  public:
    /**
     * @brief Construct the view
     */
    static std::shared_ptr<ConnectedDeviceView> create ();
    ~ConnectedDeviceView () {}

    ConnectedDeviceView (const ConnectedDeviceView&) = delete;
    ConnectedDeviceView& operator= (const ConnectedDeviceView&) = delete;

    friend Gtk::Builder;

  private:
    ConnectedDeviceView (BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder> glade_ref);

    Glib::RefPtr<Gtk::Builder> m_builder;
};

} // namespace Views
} // namespace App
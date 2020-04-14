/* application.h
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
#include <list>
#include "window.h"
#include "widgets/popover-window.h"

namespace App {

// forward declarations
namespace Models {
class ConnectedDevices;
class UnavailableDevices;
class AvailableDevices;
} // namespace Models

/**
 * @brief The app's main application class
 *
 * The @p Application manages the lifetime of the application by providing a mainloop and managing
 * the app's windows
 */
class Application : public Gtk::Application {
  public:
    /**
     * @brief Create a new application
     */
    static Glib::RefPtr<Application> create ();
    ~Application () {}

    Application (const Application&) = delete;
    Application& operator= (const Application&) = delete;

  protected:
    Application ();

    void on_startup () override;
    void on_activate () override;
    int  on_command_line (const Glib::RefPtr<Gio::ApplicationCommandLine>& command_line) override;

  private:
    std::shared_ptr<Window>                  m_window;
    Glib::RefPtr<Models::ConnectedDevices>   m_connected_devices;
    Glib::RefPtr<Models::UnavailableDevices> m_unavailable_devices;
    Glib::RefPtr<Models::AvailableDevices>   m_available_devices;
    Glib::ustring                            m_open_dev_id;

    std::list<std::shared_ptr<Widgets::PopoverWindow>> m_popovers;
};

} // namespace App
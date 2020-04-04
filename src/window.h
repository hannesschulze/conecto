/* window.h
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
#include <memory>

namespace App {

/**
 * @brief Conecto's main window
 * 
 * Class responsible for creating the main application window (not the dock-popover) that
 * will contain a list of devices as well as other widgets. Because it is a toplevel window,
 * it is managed using a std::shared_ptr
 */
class Window : public Gtk::ApplicationWindow {
  public:
    /**
     * @brief Create a new window and restore its position and size
     *
     * The window should be attached to a @p Gtk::Application using @p Gtk::Application::add_window
     */
    static std::shared_ptr<Window> create ();
    ~Window () {}

    Window (const Window&) = delete;
    Window& operator= (const Window&) = delete;

  protected:
    Window ();

    // Called when the window should be closed, saves the window's position and size
    bool on_delete_event (GdkEventAny* event) override;
};

} // namespace App
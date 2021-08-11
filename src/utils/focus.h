/* focus.h
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

#include <gdkmm/window.h>

namespace App {
namespace Utils {

/**
 * Focus helper methods (stolen from https://github.com/aprilis/messenger/blob/master/src/FocusGrabber.vala)
 */
class Focus {
  public:
    /**
     * @brief Utility for grabbing focus
     */
    static void grab (const Glib::RefPtr<Gdk::Window>& window, bool keyboard = true, bool pointer = true,
                      bool owner_events = true);

    /**
     * @brief Utility for grabbing focus
     */
    static void ungrab (bool keyboard = true, bool pointer = true);

  private:
    static bool try_grab_window (const Glib::RefPtr<Gdk::Window>& window, bool keyboard, bool pointer,
                                 bool owner_events);
};

} // namespace Utils
} // namespace App
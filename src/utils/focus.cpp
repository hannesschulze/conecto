/* focus.cpp
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

#include "focus.h"
#include <gtkmm.h>

using namespace App::Utils;

void
Focus::grab (const Glib::RefPtr<Gdk::Window>& window, bool keyboard, bool pointer, bool owner_events)
{
    if (!keyboard && !pointer) return;

    window->raise ();
    window->focus (GDK_CURRENT_TIME);

    if (!try_grab_window (window, keyboard, pointer, owner_events)) {
        auto i = std::make_shared<int> (0);
        Glib::signal_timeout ().connect (
                [i, window, keyboard, pointer, owner_events] () {
                    if (++(*i) >= 100) {
                        g_warning ("Grab failed");
                        return false;
                    }
                    return !try_grab_window (window, keyboard, pointer, owner_events);
                },
                100);
    }
}

void
Focus::ungrab (bool keyboard, bool pointer)
{
    auto display = Gdk::Display::get_default ();
    auto manager = display->get_device_manager ();

    auto list = manager->list_devices (Gdk::DEVICE_TYPE_MASTER);

    for (auto& device : list) {
        if ((device->property_input_source ().get_value () == Gdk::SOURCE_KEYBOARD && keyboard) ||
            (device->property_input_source ().get_value () != Gdk::SOURCE_KEYBOARD && pointer))
            device->ungrab (GDK_CURRENT_TIME);
    }
}

bool
Focus::try_grab_window (const Glib::RefPtr<Gdk::Window>& window, bool keyboard, bool pointer, bool owner_events)
{
    auto display = Gdk::Display::get_default ();
    auto manager = display->get_device_manager ();

    bool grabbed_all = true;

    auto list = manager->list_devices (Gdk::DEVICE_TYPE_MASTER);
    for (auto& device : list) {
        if ((device->property_input_source ().get_value () == Gdk::SOURCE_KEYBOARD && keyboard) ||
            (device->property_input_source ().get_value () != Gdk::SOURCE_KEYBOARD && pointer)) {
            Gdk::GrabStatus status = device->grab (window, Gdk::OWNERSHIP_APPLICATION, owner_events,
                                                   Gdk::ALL_EVENTS_MASK, GDK_CURRENT_TIME);

            if (status != Gdk::GRAB_SUCCESS) grabbed_all = false;
        }
    }

    if (grabbed_all) return true;

    ungrab (keyboard, pointer);

    return false;
}
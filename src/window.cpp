/* window.cpp
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

#include "window.h"
#include "controllers/settings-manager.h"
#ifdef ENABLE_PLANK_SUPPORT
#include "controllers/dock-item-manager.h"
#endif

using namespace App;

Window::Window (const Glib::RefPtr<Models::ConnectedDevices>&   connected_devices,
                const Glib::RefPtr<Models::UnavailableDevices>& unavailable_devices,
                const Glib::RefPtr<Models::AvailableDevices>&   available_devices)
    : Gtk::ApplicationWindow ()
    , m_paned (Gtk::ORIENTATION_HORIZONTAL)
    , m_devices_list (connected_devices, unavailable_devices, available_devices)
    , m_active_device (connected_devices, unavailable_devices, available_devices)
    , m_header_bar (Widgets::HeaderBar::create (connected_devices, unavailable_devices, available_devices,
                                                *m_active_device.get_connected_device_stack ()))
{
    // Set size and position
    set_size_request (640, 480);
    int x = SETTINGS.get_window_x ();
    int y = SETTINGS.get_window_y ();
    int width = SETTINGS.get_window_width ();
    int height = SETTINGS.get_window_height ();
    if (x != -1 && y != -1 && width != -1 && height != -1) {
        move (x, y);
        set_default_size (width, height);
    } else {
        set_default_size (640, 480);
    }

    // Set up CSS
    auto main_provider = Gtk::CssProvider::create ();
    auto fallback_provider = Gtk::CssProvider::create ();
    main_provider->load_from_resource ("/com/github/hannesschulze/conecto/css/style.css");
    fallback_provider->load_from_resource ("/com/github/hannesschulze/conecto/css/fallback.css");

    Gtk::StyleContext::add_provider_for_screen (Gdk::Screen::get_default (), main_provider,
                                                GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    Gtk::StyleContext::add_provider_for_screen (Gdk::Screen::get_default (), fallback_provider,
                                                GTK_STYLE_PROVIDER_PRIORITY_FALLBACK);

    // Set up the view
    m_paned.set_position (155);
    m_paned.pack1 (m_devices_list, false, false);
    m_paned.add2 (m_active_device);
    add (m_paned);

    // Set titlebar
    set_titlebar (*m_header_bar);

    show_all ();
}

std::shared_ptr<Window>
Window::create (const Glib::RefPtr<Models::ConnectedDevices>&   connected_devices,
                const Glib::RefPtr<Models::UnavailableDevices>& unavailable_devices,
                const Glib::RefPtr<Models::AvailableDevices>&   available_devices)
{
    return std::shared_ptr<Window> (new Window (connected_devices, unavailable_devices, available_devices));
}

bool
Window::on_delete_event (GdkEventAny* event)
{
    int x, y, width, height;
    get_position (x, y);
    get_size (width, height);

    SETTINGS.set_window_x (x);
    SETTINGS.set_window_y (y);
    SETTINGS.set_window_width (width);
    SETTINGS.set_window_height (height);

    return false;
}

void
Window::on_show ()
{
    Gtk::ApplicationWindow::on_show ();
#ifdef ENABLE_PLANK_SUPPORT
    Glib::signal_idle ().connect_once ([this] () { DOCK_ITEMS.listen (); });
#endif
}
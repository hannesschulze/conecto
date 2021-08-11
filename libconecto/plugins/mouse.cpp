/* mouse.cpp
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

#include "mouse.h"
#include "device.h"
#include "network-packet.h"
#include <atspi/atspi.h>
#include <gdkmm/monitor.h>

using namespace Conecto::Plugins;

namespace {

constexpr char REQUEST[] = "kdeconnect.mousepad.request";
constexpr char PACKET_TYPE[] = "kdeconnect.mousepad";

void
move_cursor_relative (double dx, double dy)
{
    GError* err = nullptr;
    atspi_generate_mouse_event (static_cast<long> (dx), static_cast<long> (dy), "rel", &err);
    if (err) {
        g_warning ("Failed to generate mouse move event: %s", err->message);
        g_error_free (err);
    }
}

void
send_key (const std::string& key)
{
    GError* err = nullptr;
    atspi_generate_keyboard_event (0, key.c_str (), ATSPI_KEY_STRING, &err);
    if (err) {
        g_warning ("Failed to generate keyboard event: %s", err->message);
        g_error_free (err);
    }
}

void
send_keysym (uint key)
{
    uint keyval = 0;
    if (key == 12)
        keyval = gdk_keyval_from_name ("Return");
    else if (key == 1)
        keyval = gdk_keyval_from_name ("BackSpace");

    if (keyval == 0) {
        g_warning ("could not identify key %u", key);
        return;
    }

    g_debug ("Keyval %x", keyval);
    GError* err = nullptr;
    atspi_generate_keyboard_event (keyval, nullptr,
                                   static_cast<AtspiKeySynthType> (ATSPI_KEY_PRESSRELEASE | ATSPI_KEY_SYM), &err);
    if (err) {
        g_warning ("Failed to generate keyboard event: %s", err->message);
        g_error_free (err);
    }
}

} // namespace

Mouse::Mouse ()
    : AbstractPacketHandler ()
{
    // Initialize atspi
    if (atspi_init () > 0) g_warning ("Couldn't initialize atspi2");
    m_display = Gdk::Display::get_default ();
    if (!m_display) g_warning ("Failed to get default display");
}

Mouse::~Mouse ()
{
    atspi_exit ();
}

std::string
Mouse::get_packet_type_virt () const noexcept
{
    return REQUEST;
}

void
Mouse::register_device_virt (const std::shared_ptr<Device>& device) noexcept
{
    m_devices[device] =
            device->signal_message ().connect (sigc::bind (sigc::mem_fun (*this, &Mouse::on_message), device));
}

void
Mouse::unregister_device_virt (const std::shared_ptr<Device>& device) noexcept
{
    if (m_devices.find (device) != m_devices.end ()) {
        m_devices.at (device).disconnect ();
        m_devices.erase (device);
    }
}

void
Mouse::on_message (const NetworkPacket& message, const std::shared_ptr<Device>& device)
{
    if (message.get_type () != PACKET_TYPE && message.get_type () != REQUEST) return;

    g_debug ("Mouse packet");

    if (!m_display) {
        g_warning ("Display not initialized");
        return;
    }

    if (message.get_body ().isMember ("singleclick")) {
        // Single click
        g_debug ("Single click");
        send_click (1);
    } else if (message.get_body ().isMember ("doubleclick")) {
        send_click (1, true);
    } else if (message.get_body ().isMember ("rightclick")) {
        send_click (3);
    } else if (message.get_body ().isMember ("middleclick")) {
        send_click (2);
    } else if (message.get_body ().isMember ("dx") && message.get_body ().isMember ("dy")) {
        // Motion/position or scrolling
        double dx = message.get_body ()["dx"].asDouble ();
        double dy = message.get_body ()["dy"].asDouble ();

        if (message.get_body ().isMember ("scroll") && message.get_body ()["scroll"].asBool ()) {
            // Scroll with variable speed
            while (dy > 3.0) {
                // Scroll down
                send_click (5);
                dy /= 4.0;
                g_debug ("Scroll down");
            }
            while (dy < -3.0) {
                // Scroll up
                send_click (4);
                dy /= 4.0;
                g_debug ("Scroll up");
            }
        } else {
            g_debug ("Position: %f x %f", dx, dy);

            move_cursor_relative (dx, dy);
        }
    } else if (message.get_body ().isMember ("key")) {
        std::string key = message.get_body ()["key"].asString ();
        g_debug ("Got key: %s", key.c_str ());
        send_key (key);
    } else if (message.get_body ().isMember ("specialKey")) {
        int keynum = message.get_body ()["specialKey"].asInt ();
        g_debug ("Got special key: %d", keynum);
        send_keysym (static_cast<uint> (keynum));
    }
}

void
Mouse::send_click (int button, bool double_click)
{
    std::string etype = "b" + std::to_string (button) + "c";
    gint        x, y;
    GdkSeat*    seat = gdk_display_get_default_seat (m_display->gobj ());
    GdkDevice*  device = gdk_seat_get_pointer (seat);
    gdk_device_get_position (device, nullptr, &x, &y);
    // TODO: Use scale factor from correct monitor
    x *= m_display->get_primary_monitor ()->get_scale_factor ();
    y *= m_display->get_primary_monitor ()->get_scale_factor ();

    GError* err = nullptr;
    atspi_generate_mouse_event (x, y, etype.c_str (), &err);
    if (err) {
        g_warning ("Failed to generate mouse click event: %s", err->message);
        g_error_free (err);
        return;
    }

    if (double_click) {
        atspi_generate_mouse_event (x, y, etype.c_str (), &err);
        if (err) {
            g_warning ("Failed to generate mouse click event: %s", err->message);
            g_error_free (err);
            return;
        }
    }
}
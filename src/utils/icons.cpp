/* icons.cpp
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

#include "icons.h"
#include <gtkmm.h>

using namespace App::Utils;

std::string
Icons::get_icon_name_for_device_type (const std::string& type)
{
    if (type == "tablet") return "computer-apple-ipad";
    return type;
}

Glib::RefPtr<Gdk::Pixbuf>
Icons::get_icon_for_device_type (const std::string& type, int size)
{
    auto icon_info = Gtk::IconTheme::get_default ()->lookup_icon (get_icon_name_for_device_type (type), size);
    if (icon_info)
        return icon_info.load_icon ();
    else
        return Gtk::IconTheme::get_default ()->load_icon ("phone", size);
}
/* icons.h
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

#include <gdkmm/pixbuf.h>

namespace App {
namespace Utils {

/**
 * Icon helper methods
 */
class Icons {
  public:
    /**
     * @brief Get the name of an icon representing a device type
     *
     * @param type The device type
     */
    static std::string get_icon_name_for_device_type (const std::string& type);

    /**
     * @brief Get an icon representing a device
     *
     * @param type The device type
     */
    static Glib::RefPtr<Gdk::Pixbuf> get_icon_for_device_type (const std::string& type, int size);
};

} // namespace Utils
} // namespace App
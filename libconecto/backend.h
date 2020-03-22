/* backend.h
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

#include "discovery.h"
#include <map>
#include <string>

namespace Conecto {

// forward declarations
class Device;

class Backend {
  public:
    static Backend& get_instance();
    ~Backend () {}

    /**
     * Start listening for new devices
     * 
     * @throw BindSocketException
     */
    void listen ();

    Backend (const Backend&) = delete;
    Backend& operator= (const Backend&) = delete;

  protected:
    Backend ();

  private:
    std::map<std::string, Device> m_devices;

    Discovery m_discovery;
};

} // namespace Conecto
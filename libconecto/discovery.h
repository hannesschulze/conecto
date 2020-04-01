/* discovery.h
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

#include "device.h"
#include <sigc++/sigc++.h>
#include <giomm/socket.h>

namespace Conecto {

// forward declarations
class Device;

class Discovery {
  public:
    Discovery ();
    ~Discovery ();

    /**
     * Start listening for new devices
     *
     * @throw BindSocketException
     */
    void listen ();

    using type_signal_device_found = sigc::signal<void, const std::shared_ptr<Device>&>;
    type_signal_device_found signal_device_found () { return m_signal_device_found; }

    Discovery (const Discovery&) = delete;
    Discovery& operator= (const Discovery&) = delete;

  private:
    bool on_packet (Glib::IOCondition condition);
    void parse_packet (std::string&& data, const Glib::RefPtr<Gio::InetAddress>& host);

    type_signal_device_found m_signal_device_found;

    Glib::RefPtr<Gio::Socket> m_socket;
};

} // namespace Conecto

/* ping.cpp
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

#include "ping.h"
#include "device.h"
#include "network-packet.h"

using namespace Conecto::Plugins;

namespace {

constexpr char PACKET_TYPE[] = "kdeconnect.ping";

} // namespace

Ping::Ping ()
    : AbstractPacketHandler ()
{
}

std::string
Ping::get_packet_type_virt () const noexcept
{
    return PACKET_TYPE;
}

void
Ping::register_device_virt (const std::shared_ptr<Device>& device) noexcept
{
    m_devices[device] =
            device->signal_message ().connect (sigc::bind (sigc::mem_fun (*this, &Ping::on_message), device));
}

void
Ping::unregister_device_virt (const std::shared_ptr<Device>& device) noexcept
{
    if (m_devices.find (device) != m_devices.end ()) {
        m_devices.at (device).disconnect ();
        m_devices.erase (device);
    }
}

void
Ping::on_message (const NetworkPacket& message, const std::shared_ptr<Device>& device)
{
    if (message.get_type () != PACKET_TYPE) return;

    m_signal_ping.emit (device);
}
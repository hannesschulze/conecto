/* battery.cpp
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

#include "battery.h"
#include "device.h"
#include "network-packet.h"

using namespace Conecto::Plugins;

namespace {

constexpr char PACKET_TYPE[] = "kdeconnect.battery";

} // namespace

Battery::Battery ()
    : AbstractPacketHandler ()
{
}

std::string
Battery::get_packet_type_virt () const noexcept
{
    return PACKET_TYPE;
}

void
Battery::register_device_virt (const std::shared_ptr<Device>& device) noexcept
{
    m_devices[device] =
            device->signal_message ().connect (sigc::bind (sigc::mem_fun (*this, &Battery::on_message), device));
}

void
Battery::unregister_device_virt (const std::shared_ptr<Device>& device) noexcept
{
    if (m_devices.find (device) != m_devices.end ()) {
        m_devices.at (device).disconnect ();
        m_devices.erase (device);
    }
}

void
Battery::on_message (const NetworkPacket& message, const std::shared_ptr<Device>& device)
{
    if (message.get_type () != PACKET_TYPE) return;

    int  level = std::min (std::max (message.get_body ()["currentCharge"].asInt (), 0), 100);
    bool charging = message.get_body ()["isCharging"].asBool ();
    m_values[device->to_unique_string ()] = std::make_tuple (level, charging);
    m_signal_battery.emit (device, level, charging);
}

std::tuple<int, bool>
Battery::get_last_value (const std::shared_ptr<Device>& device) const
{
    if (m_values.find (device->to_unique_string ()) == m_values.end ()) return std::make_tuple (0, false);
    return m_values.at (device->to_unique_string ());
}
/* battery.h
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

#include "abstract-packet-handler.h"
#include <map>
#include <sigc++/sigc++.h>

namespace Conecto {

class NetworkPacket;

namespace Plugins {

/**
 * @brief Battery level plugin
 */
class Battery : public AbstractPacketHandler {
  public:
    /**
     * Create a new instance of this plugin
     */
    Battery ();
    ~Battery () {}

    /**
     * Get the last battery level and charging flag for a device
     */
    std::tuple<int, bool> get_last_value (const std::shared_ptr<Device>& device) const;

    /**
     * @param device The device which sent the battery level update
     * @param level The new battery level [0..100]
     * @param charging true if the battery is currently charging
     */
    using type_signal_battery = sigc::signal<void, const std::shared_ptr<Device>& /* device */, int /* level */, bool /* charging */>;
    /**
     * Emitted after receiving a battery level update from a device
     */
    type_signal_battery signal_battery () { return m_signal_battery; }

    Battery (const Battery&) = delete;
    Battery& operator= (const Battery&) = delete;

  protected:
    // packet handler
    void on_message (const NetworkPacket& message, const std::shared_ptr<Device>& device);

    // overrides
    std::string get_packet_type_virt () const noexcept override;
    void        register_device_virt (const std::shared_ptr<Device>& device) noexcept override;
    void        unregister_device_virt (const std::shared_ptr<Device>& device) noexcept override;

  private:
    std::map<std::shared_ptr<Device>, sigc::connection> m_devices;
    std::map<std::string, std::tuple<int, bool>>        m_values;

    type_signal_battery m_signal_battery;
};

} // namespace Plugins
} // namespace Conecto
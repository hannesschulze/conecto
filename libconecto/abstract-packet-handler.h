/* abstract-packet-handler.h
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

#include <string>
#include <memory>

namespace Conecto {

// forward declarations
class Device;

/**
 * Abstract base class for all packet handlers
 */
class AbstractPacketHandler {
  public:
    AbstractPacketHandler () {}
    virtual ~AbstractPacketHandler () {}

    /**
     * Get the packet type this class is handling
     *
     * @return The packet type as a string
     */
    std::string get_packet_type () const noexcept { return get_packet_type_virt (); }
    /**
     * Register a new device for this handler
     *
     * @param device The device
     */
    void register_device (const std::shared_ptr<Device>& device) noexcept { return register_device_virt (device); }
    /**
     * Unregister a device for this handler
     *
     * @param device The device
     */
    void unregister_device (const std::shared_ptr<Device>& device) noexcept { return unregister_device_virt (device); }

    AbstractPacketHandler (const AbstractPacketHandler&) = delete;
    AbstractPacketHandler& operator= (const AbstractPacketHandler&) = delete;

  protected:
    virtual std::string get_packet_type_virt () const noexcept = 0;
    virtual void        register_device_virt (const std::shared_ptr<Device>& device) noexcept = 0;
    virtual void        unregister_device_virt (const std::shared_ptr<Device>& device) noexcept = 0;
};

} // namespace Conecto
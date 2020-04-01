/* network-packet.h
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
#include <json/json.h>
#include <list>

namespace Conecto {

namespace Constants {
static const std::string TYPE_IDENTITY = "kdeconnect.identity";
static const std::string TYPE_PAIR = "kdeconnect.pair";
static const std::string TYPE_ENCRYPTED = "kdeconnect.encrypted";
} // namespace Constants

class NetworkPacket {
  public:
    /**
     * Create a new @p NetworkPacket with a specified type, body and id
     *
     * @param type The packet type as a string
     * @param body The JSON body
     * @param id A unique identifier
     */
    NetworkPacket (const std::string& type, const Json::Value& body, const int64_t& id = 0) noexcept;
    /**
     * Create a new @p NetworkPacket from a stringified JSON value
     *
     * @param data The JSON data
     * @throw MalformedPacketException
     */
    NetworkPacket (const std::string& data);
    /**
     * Create a new network packet with the type @p Constants::TYPE_PAIR
     */
    static std::shared_ptr<NetworkPacket> create_pair (bool pair = true);
    /**
     * Create a new network packet with the type @p Constants::TYPE_IDENTITY
     */
    static std::shared_ptr<NetworkPacket> create_identity (const std::string& name, const std::string& device_id,
                                                           const std::list<std::string>& in_interfaces,
                                                           const std::list<std::string>& out_interfaces,
                                                           const std::string&            device_type = "desktop");
    ~NetworkPacket () {}

    /**
     * A wrapper structure for payload transfer information
     */
    struct Payload {
        Payload (const uint64_t& size, uint port)
            : size (size)
            , port (port)
        {
        }
        uint64_t size;
        uint     port;
    };

    const std::string&             get_type () const noexcept;
    const int64_t&                 get_id () const noexcept;
    const Json::Value&             get_body () const noexcept;
    std::shared_ptr<const Payload> get_payload () const noexcept;
    void                           set_payload (const std::shared_ptr<Payload>& payload) noexcept;

    std::string serialize () const noexcept;

    NetworkPacket (const NetworkPacket&) = delete;
    NetworkPacket& operator= (const NetworkPacket&) = delete;

  private:
    std::string              m_type;
    int64_t                  m_id;
    Json::Value              m_body;
    std::shared_ptr<Payload> m_payload;
};

} // namespace Conecto

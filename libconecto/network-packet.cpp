/* network-packet.cpp
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

#include "network-packet.h"
#include "exceptions.h"
#include <gio/gio.h>

using namespace Conecto;

namespace {

constexpr int PROTOCOL_VERSION = 7;

}

NetworkPacket::NetworkPacket (const std::string& type, const Json::Value& body, const int64_t& id) noexcept
    : m_type (type)
    , m_id (id)
    , m_body (body)
{
    if (m_id == 0)
        m_id = g_get_real_time () / 1000;
}

NetworkPacket::NetworkPacket (const std::string& data)
{
    Json::CharReaderBuilder builder;
    std::unique_ptr<Json::CharReader> reader (builder.newCharReader ());

    Json::Value val;
    std::string err;
    if (!reader->parse (data.c_str (), data.c_str () + data.size (), &val, &err) || !val.isObject ())
        throw MalformedPacketException ("Invalid JSON");

    // Object needs to have these fields
    if (!val["type"].isString ())
        throw MalformedPacketException ("Missing 'type' member");
    if (!val["id"].isInt64 ())
        throw MalformedPacketException ("Missing 'id' member");
    if (!val["body"].isObject ())
        throw MalformedPacketException ("Missing 'body' member");

    m_type = val["type"].asString ();
    m_id = val["id"].asInt64 ();
    m_body = val["body"];

    // Ignore payload info for encrypted packets
    if (m_type == Constants::TYPE_ENCRYPTED) return;

    if (val["payloadSize"].isInt () && val["payloadTransferInfo"].isObject ()) {
        int size = val["payloadSize"].asInt ();
        Json::Value transfer_info = val["payloadTransferInfo"];
        int port = 0;
        if (!transfer_info["port"].asInt ())
            g_warning ("No payload transfer info");
        else
            port = transfer_info["port"].asInt ();
        
        if (size != 0 && port != 0)
            m_payload = std::make_shared<Payload> (size, port);
    }
}

std::shared_ptr<NetworkPacket>
NetworkPacket::create_pair (bool pair)
{
    Json::Value body (Json::objectValue);
    body["pair"] = pair;

    return std::make_shared<NetworkPacket> (Constants::TYPE_PAIR, body);
}

std::shared_ptr<NetworkPacket>
NetworkPacket::create_identity (const std::string& name, const std::string& device_id, const std::list<std::string>& in_interfaces,
                                const std::list<std::string>& out_interfaces, const std::string& device_type)
{
    Json::Value body (Json::objectValue);
    body["deviceName"] = name;
    body["deviceId"] = device_id;
    body["deviceType"] = device_type;
    Json::Value in_array (Json::arrayValue);
    for (const auto& interface : in_interfaces)
        in_array.append (interface);
    body["SupportedIncomingInterfaces"] = std::move (in_array);
    Json::Value out_array (Json::arrayValue);
    for (const auto& interface : out_interfaces)
        out_array.append (interface);
    body["SupportedOutgoingInterfaces"] = std::move (out_array);
    body["protocolVersion"] = PROTOCOL_VERSION;

    return std::make_shared<NetworkPacket> (Constants::TYPE_IDENTITY, body);
}

const std::string&
NetworkPacket::get_type () const noexcept
{
    return m_type;
}

const int64_t&
NetworkPacket::get_id () const noexcept
{
    return m_id;
}

const Json::Value&
NetworkPacket::get_body () const noexcept
{
    return m_body;
}

std::shared_ptr<const NetworkPacket::Payload>
NetworkPacket::get_payload () const noexcept
{
    return m_payload;
}

void
NetworkPacket::set_payload (const std::shared_ptr<Payload>& payload) noexcept
{
    m_payload = payload;
}

std::string
NetworkPacket::serialize () const noexcept
{
    Json::Value res (Json::objectValue);

    res["type"] = m_type;
    res["id"] = m_id;
    res["body"] = m_body;
    if (m_payload) {
        res["payloadSize"] = m_payload->size;
        Json::Value transfer_info (Json::objectValue);
        transfer_info["port"] = m_payload->port;
        res["payloadTransferInfo"] = std::move (transfer_info);
    }

    Json::StreamWriterBuilder builder;
    builder["indentation"] = ""; // no indentation
    return Json::writeString (builder, res);
}
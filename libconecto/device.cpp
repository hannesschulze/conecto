/* device.cpp
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

#include "device.h"
#include "exceptions.h"

using namespace Conecto;

Device::Device ()
    : m_device_id ("")
    , m_device_name ("")
    , m_device_type ("")
    , m_protocol_version (7)
    , m_tcp_port (1714)
    , m_is_paired (false)
    , m_allowed (false)
    , m_is_active (false)
    , m_certificate_fingerprint ("")
    , m_pair_in_progress (false)
    , m_pair_timeout_source (0)
{
}

Device::Device (const NetworkPacket& packet, const Glib::RefPtr<Gio::InetAddress>& host)
    : Device ()
{
    const Json::Value& body = packet.get_body ();
    if (!body["deviceName"].isString () || !body["deviceId"].isString () ||
            !body["deviceType"].isString () || !body["protocolVersion"].isInt () ||
            !body["tcpPort"].isInt () || !body["incomingCapabilities"].isArray () ||
            !body["outgoingCapabilities"].isArray ())
        throw MalformedPacketException ("Missing member(s)");

    m_host = host;
    m_device_name = body["deviceName"].asString ();
    m_device_id = body["deviceId"].asString ();
    m_device_type = body["deviceType"].asString ();
    m_protocol_version = body["protocolVersion"].asInt ();
    m_tcp_port = body["tcpPort"].asInt ();
    for (Json::Value::ArrayIndex i = 0; i < body["incomingCapabilities"].size (); i++)
        if (body["incomingCapabilities"][i].isString ())
            m_incoming_capabilities.push_back (body["incomingCapabilities"][i].asString ());
    for (Json::Value::ArrayIndex i = 0; i < body["outgoingCapabilities"].size (); i++)
        if (body["outgoingCapabilities"][i].isString ())
            m_outgoing_capabilities.push_back (body["outgoingCapabilities"][i].asString ());

    g_debug ("New device: %s", to_string ().c_str ());
}

const std::string&
Device::get_device_id () const noexcept
{
    return m_device_id;
}

const std::string&
Device::get_device_name () const noexcept
{
    return m_device_name;
}

const std::string&
Device::get_device_type () const noexcept
{
    return m_device_type;
}

const uint&
Device::get_protocol_version () const noexcept
{
    return m_protocol_version;
}

const uint&
Device::get_tcp_port () const noexcept
{
    return m_tcp_port;
}

const Glib::RefPtr<Gio::InetAddress>&
Device::get_host () const noexcept
{
    return m_host;
}

const bool&
Device::get_is_paired () const noexcept
{
    return m_is_paired;
}

const bool&
Device::get_allowed () const noexcept
{
    return m_allowed;
}

const bool&
Device::get_is_active () const noexcept
{
    return m_is_active;
}

const std::list<std::string>&
Device::get_outgoing_capabilities () const noexcept
{
    return m_outgoing_capabilities;
}

const std::list<std::string>&
Device::get_incoming_capabilities () const noexcept
{
    return m_incoming_capabilities;
}

Glib::RefPtr<Gio::TlsCertificate>
Device::get_certificate () const noexcept
{
    return m_certificate;
}

std::string
Device::get_certificate_pem () const noexcept
{
    if (!m_certificate)
        return "";
    return m_certificate->property_certificate_pem ().get_value ();
}

const std::string&
Device::get_certificate_fingerprint () const noexcept
{
    return m_certificate_fingerprint;
}

void
Device::set_allowed (bool allowed) noexcept
{
    m_allowed = allowed;
}

void
Device::set_certificate (Glib::RefPtr<Gio::TlsCertificate> certificate) noexcept
{
    m_certificate = certificate;
}

std::string
Device::to_string () const noexcept
{
    std::stringstream res;
    res << m_device_id << "-" << m_device_name << "-" << m_device_type << "-" << m_protocol_version;
    return res.str ();
}

std::string
Device::to_unique_string () const noexcept
{
    std::string dev = to_string ();
    std::replace (dev.begin (), dev.end (), '-', ' ');
    return dev;
}
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
#include "backend.h"
#include "crypt.h"
#include "communication-channel.h"
#include <glibmm/miscutils.h>
#include <glibmm/main.h>
#include <iomanip>

using namespace Conecto;

namespace {

constexpr int PAIR_TIMEOUT = 30;

} // namespace

Device::Device ()
    : m_device_id ("")
    , m_device_name ("")
    , m_device_type ("")
    , m_protocol_version (7)
    , m_tcp_port (1714)
    , m_is_paired (false)
    , m_is_active (false)
    , m_certificate_fingerprint ("")
    , m_pair_in_progress (false)
    , m_pair_requested (false)
{
}

std::shared_ptr<Device>
Device::create_from_packet (const NetworkPacket& packet, const Glib::RefPtr<Gio::InetAddress>& host)
{
    std::shared_ptr<Device> res (new Device);

    const Json::Value& body = packet.get_body ();
    if (!body["deviceName"].isString () || !body["deviceId"].isString () || !body["deviceType"].isString () ||
        !body["protocolVersion"].isInt () || !body["tcpPort"].isInt () || !body["incomingCapabilities"].isArray () ||
        !body["outgoingCapabilities"].isArray ())
        throw MalformedPacketException ("Missing member(s)");

    res->m_host = host;
    res->m_device_name = body["deviceName"].asString ();
    res->m_device_id = body["deviceId"].asString ();
    res->m_device_type = body["deviceType"].asString ();
    res->m_protocol_version = body["protocolVersion"].asInt ();
    res->m_tcp_port = body["tcpPort"].asInt ();
    for (Json::Value::ArrayIndex i = 0; i < body["incomingCapabilities"].size (); i++)
        if (body["incomingCapabilities"][i].isString ())
            res->m_incoming_capabilities.push_back (body["incomingCapabilities"][i].asString ());
    for (Json::Value::ArrayIndex i = 0; i < body["outgoingCapabilities"].size (); i++)
        if (body["outgoingCapabilities"][i].isString ())
            res->m_outgoing_capabilities.push_back (body["outgoingCapabilities"][i].asString ());

    g_debug ("New device: %s", res->to_string ().c_str ());
    return res;
}

std::shared_ptr<Device>
Device::create_from_cache (Glib::KeyFile& cache, const std::string& name)
{
    std::shared_ptr<Device> res (new Device);

    res->m_device_id = cache.get_string (name, "deviceId");
    res->m_device_name = cache.get_string (name, "deviceName");
    res->m_device_type = cache.get_string (name, "deviceType");
    res->m_protocol_version = cache.get_integer (name, "protocolVersion");
    res->m_tcp_port = (uint) cache.get_integer (name, "tcpPort");
    std::string last_ip_str = cache.get_string (name, "lastIPAddress");
    g_debug ("Last known address: %s:%u", last_ip_str.c_str (), res->m_tcp_port);
    res->m_is_paired = cache.get_boolean (name, "paired");
    try {
        std::string cached_certificate = cache.get_string (name, "certificate");
        if (cached_certificate != std::string ()) {
            GError*          err = nullptr;
            GTlsCertificate* cert =
                    g_tls_certificate_new_from_pem (cached_certificate.c_str (), cached_certificate.size (), &err);
            g_assert (err == nullptr);
            res->update_certificate (Glib::wrap (cert));
        }
    } catch (Glib::KeyFileError& err) {
        if (err.domain () == G_KEY_FILE_ERROR_KEY_NOT_FOUND)
            g_warning ("Device %s using older cache format", res->m_device_id.c_str ());
        else
            throw err;
    }
    res->m_outgoing_capabilities = cache.get_string_list (name, "outgoing_capabilities");
    res->m_incoming_capabilities = cache.get_string_list (name, "incoming_capabilities");
    res->m_host = Gio::InetAddress::create (last_ip_str);
    if (!res->m_host) {
        g_debug ("Failed to parse last known IP address (%s) for device %s", last_ip_str.c_str (), name.c_str ());
        throw InvalidIpAddressException (last_ip_str);
    }

    return res;
}

void
Device::to_cache (Glib::KeyFile& cache, const std::string& name) const noexcept
{
    cache.set_string (name, "deviceId", get_device_id ());
    cache.set_string (name, "deviceName", get_device_name ());
    cache.set_string (name, "deviceType", get_device_type ());
    cache.set_integer (name, "protocolVersion", (int) get_protocol_version ());
    cache.set_integer (name, "tcpPort", (int) get_tcp_port ());
    cache.set_string (name, "lastIPAddress", get_host ()->to_string ());
    cache.set_boolean (name, "paired", get_is_paired ());
    cache.set_string (name, "certificate", get_certificate_pem ());
    cache.set_string_list (name, "outgoing_capabilities", get_outgoing_capabilities ());
    cache.set_string_list (name, "incoming_capabilities", get_incoming_capabilities ());
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

Glib::RefPtr<Gio::InetAddress>
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
Device::get_pair_requested () const noexcept
{
    return m_pair_requested;
}

const bool&
Device::get_pair_in_progress () const noexcept
{
    return m_pair_in_progress;
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
    if (!m_certificate) return "";
    return m_certificate->property_certificate_pem ().get_value ();
}

const std::string&
Device::get_certificate_fingerprint () const noexcept
{
    return m_certificate_fingerprint;
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

void
Device::greet (std::function<void ()> cb) noexcept
{
    std::string host_name = g_get_host_name ();
    std::string user = Glib::get_user_name ();
    auto        packet = NetworkPacket::create_identity (user + "@" + host_name, host_name,
                                                  Backend::get_instance ().get_handler_interfaces (),
                                                  Backend::get_instance ().get_handler_interfaces ());
    m_channel->send (*packet);

    // Switch to secure channel
    m_channel->secure (m_certificate, [this, cb] (bool success) {
        g_info ("Secure: %s", success ? "true" : "false");
        if (success) {
            update_certificate (m_channel->get_peer_certificate ());
        } else {
            g_warning ("Failed to enable secure channel");
            close_and_cleanup ();
        }
    });
}

void
Device::pair (bool expect_response) noexcept
{
    if (!m_host) return;
    g_debug ("Start pairing");

    if (expect_response == true) {
        m_pair_in_progress = true;
        m_signal_pair_request.emit ();
        // Pairing timeout
        m_pair_timeout_connection =
                Glib::signal_timeout ().connect_seconds (sigc::mem_fun (*this, &Device::on_pair_timeout), PAIR_TIMEOUT);
    }

    // Send request
    m_channel->send (*NetworkPacket::create_pair ());
}

bool
Device::on_pair_timeout ()
{
    g_warning ("Pair request timeout");
    m_pair_timeout_connection = sigc::connection ();

    // Handle failed pairing
    handle_pair (false);

    // Remove timeout source
    return false;
}

void
Device::maybe_pair () noexcept
{
    if (!m_is_paired) {
        if (!m_pair_in_progress) pair (true);
    } else {
        // We are already paired
        handle_pair (true);
    }
}

void
Device::unpair () noexcept
{
    if (!m_host) return;
    g_debug ("Unpairing");

    // Send request
    if (m_channel) m_channel->send (*NetworkPacket::create_pair (false));
    handle_pair (false);
}

void
Device::send_pair_response (bool accept) noexcept
{
    if (!m_pair_requested) return;

    handle_pair (accept);
}

void
Device::activate () noexcept
{
    if (m_channel) g_debug ("Device %s is already active", to_string ().c_str ());

    m_channel = std::make_unique<CommunicationChannel> (m_host, static_cast<uint16_t> (m_tcp_port));
    m_channel->signal_disconnected ().connect ([this] () {
        // Channel got disconnected
        g_debug ("Channel disconnected");
        close_and_cleanup ();
    });
    m_channel->signal_packet_received ().connect (sigc::mem_fun (*this, &Device::on_packet_received));
    m_channel->open ([this] (bool success) {
        g_debug ("Channel opened: %s", success ? "true" : "false");
        m_signal_connected.emit ();

        if (success)
            greet ([] () {});
        else
            // Failed to open channel
            channel_closed_cleanup ();
    });
    m_is_active = true;
}

void
Device::deactivate () noexcept
{
    if (m_channel) close_and_cleanup ();
}

void
Device::on_packet_received (const NetworkPacket& packet)
{
    g_debug ("Got packet");
    if (packet.get_type () == PacketTypes::TYPE_PAIR) {
        // Pairing
        bool pair = packet.get_body ()["pair"].asBool ();
        handle_pair (pair);
    } else {
        if (!m_is_paired) {
            // We are unpaired but received a packet -> tell the device that we are unpaired
            g_warning ("Not paired but got a packet, assuming device is unpaired");
            unpair ();
        }

        // Emit signal
        m_signal_message.emit (packet);
    }
}

void
Device::handle_pair (bool pair) noexcept
{
    if (!m_pair_timeout_connection.empty ()) {
        m_pair_timeout_connection.disconnect ();
        m_pair_timeout_connection = sigc::connection ();
    }

    g_debug ("Paired: %s", pair ? "true" : "false");
    if (m_pair_in_progress) {
        // Respond if host initiated pairing
        if (pair) {
            g_debug ("Device paired, pairing complete");
            m_is_paired = true;
            m_pair_requested = false;
        } else {
            g_warning ("Pairing rejected by device");
            m_is_paired = false;
            m_pair_requested = false;
        }
        // Pair complete
        m_pair_in_progress = false;
    } else if (m_pair_requested) {
        m_pair_requested = false;
        m_is_paired = pair;
        if (pair)
            this->pair (false);
        else
            m_channel->send (*NetworkPacket::create_pair (false));
    } else {
        g_debug ("Unsolicited pair change from device");
        if (pair) {
            // Show pair request
            m_is_paired = false;
            m_pair_requested = true;
            m_signal_pair_request.emit ();
            return;
        } else {
            // Unpair from device
            m_is_paired = false;
            m_pair_requested = false;
        }
    }

    m_signal_paired.emit (m_is_paired);
}

void
Device::close_and_cleanup () noexcept
{
    m_channel->close ();
    channel_closed_cleanup ();
}

void
Device::channel_closed_cleanup () noexcept
{
    g_debug ("Device cleanup");
    m_channel.reset ();

    m_is_active = false;

    // Emit signal
    m_signal_disconnected.emit ();
}

void
Device::merge_capabilities (std::vector<std::string>& added, std::vector<std::string>& removed) noexcept
{
    std::list<std::string> capabilities;
    for (const auto& cap : m_outgoing_capabilities) capabilities.push_back (cap);
    for (const auto& cap : m_incoming_capabilities) capabilities.push_back (cap);

    added.clear ();
    added.reserve (capabilities.size ());
    for (const auto& cap : capabilities) added.push_back (cap);

    // TODO: Simplify capability names by removing .request suffix
    for (const auto& cap : m_capabilities) {
        auto it = std::find (added.begin (), added.end (), cap);
        if (it != added.end ()) added.erase (it);
    }

    removed.clear ();
    removed.reserve (m_capabilities.size ());
    for (const auto& cap : m_capabilities) removed.push_back (cap);
    for (const auto& cap : capabilities) {
        auto it = std::find (removed.begin (), removed.end (), cap);
        if (it != removed.end ()) removed.erase (it);
    }

    m_capabilities.swap (capabilities);
}

void
Device::update_certificate (const Glib::RefPtr<Gio::TlsCertificate>& certificate) noexcept
{
    m_certificate = certificate;

    // Prepare fingerprint
    std::stringstream stream;
    stream << "sha1:";
    auto fingerprint = Crypt::fingerprint_certificate (certificate->property_certificate_pem ().get_value ());
    for (const auto& b : fingerprint) stream << std::setfill ('0') << std::setw (2) << std::hex << unsigned (b);
    m_certificate_fingerprint = stream.str ();
}

void
Device::send (const NetworkPacket& packet) noexcept
{
    // TODO: Queue messages
    if (m_channel) m_channel->send (packet);
}

bool
Device::has_capability_handler (const std::string& capability) const noexcept
{
    return m_handlers.find (capability) != m_handlers.end ();
}

void
Device::register_capability_handler (const std::string&                            capability,
                                     const std::shared_ptr<AbstractPacketHandler>& handler)
{
    if (has_capability_handler (capability)) throw PacketHandlerAlreadyRegisteredException (capability);

    m_handlers.insert ({ capability, handler });
    handler->register_device (shared_from_this ());
}

void
Device::unregister_capability_handler (const std::string& capability)
{
    if (!has_capability_handler (capability)) throw PacketHandlerNotRegisteredException (capability);

    m_handlers.at (capability)->unregister_device (shared_from_this ());
    m_handlers.erase (capability);
}

void
Device::update (const Device& device) noexcept
{
    m_outgoing_capabilities = device.m_outgoing_capabilities;
    m_incoming_capabilities = device.m_incoming_capabilities;

    std::vector<std::string> added;
    std::vector<std::string> removed;
    merge_capabilities (added, removed);

    for (const auto& cap : added) {
        g_debug ("Added: %s", cap.c_str ());
        m_signal_capability_added.emit (cap);
    }

    for (const auto& cap : removed) {
        g_debug ("Removed: %s", cap.c_str ());
        m_signal_capability_removed.emit (cap);
        unregister_capability_handler (cap);
    }

    if (m_host && m_host->to_string () != device.m_host->to_string ()) {
        g_debug ("Host address changed from %s to %s", m_host->to_string ().c_str (),
                 device.m_host->to_string ().c_str ());
        deactivate ();

        m_host = device.m_host;
        m_tcp_port = device.m_tcp_port;
    }
}
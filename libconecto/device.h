/* device.h
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

#include "network-packet.h"
#include <giomm/inetaddress.h>
#include <giomm/tlscertificate.h>

namespace Conecto {

// forward declarations
class CommunicationChannel;

class Device {
  public:
    /**
     * Default constructor
     */
    Device ();
    /**
     * Constructs a new Device wrapper based on an identity packet
     *
     * @param packet An identity packet
     * @param host Source host that the packet came from
     * @throw MalformedPacketException
     */
    Device (const NetworkPacket& packet, const Glib::RefPtr<Gio::InetAddress>& host);
    ~Device () {}

    const std::string& get_device_id () const noexcept;
    const std::string& get_device_name () const noexcept;
    const std::string& get_device_type () const noexcept;
    const uint& get_protocol_version () const noexcept;
    const uint& get_tcp_port () const noexcept;
    const Glib::RefPtr<Gio::InetAddress>& get_host () const noexcept;
    const bool& get_is_paired () const noexcept;
    const bool& get_allowed () const noexcept;
    const bool& get_is_active () const noexcept;
    const std::list<std::string>& get_outgoing_capabilities () const noexcept;
    const std::list<std::string>& get_incoming_capabilities () const noexcept;
    Glib::RefPtr<Gio::TlsCertificate> get_certificate () const noexcept;
    std::string get_certificate_pem () const noexcept;
    const std::string& get_certificate_fingerprint () const noexcept;

    void set_allowed (bool allowed) noexcept;
    void set_certificate (Glib::RefPtr<Gio::TlsCertificate> certificate) noexcept;

    std::string to_string () const noexcept;
    std::string to_unique_string () const noexcept;

    using type_signal_paired = sigc::signal<void, bool /* pair */>;
    using type_signal_connected = sigc::signal<void>;
    using type_signal_disconnected = sigc::signal<void>;
    using type_signal_message = sigc::signal<void, const NetworkPacket& /* message */>;
    using type_signal_capability_added = sigc::signal<void, const std::string& /* capability */>;
    using type_signal_capability_removed = sigc::signal<void, const std::string& /* capability */>;
    type_signal_paired signal_paired () { return m_signal_paired; }
    type_signal_connected signal_connected () { return m_signal_connected; }
    type_signal_disconnected signal_disconnected () { return m_signal_disconnected; }
    type_signal_message signal_message () { return m_signal_message; }
    type_signal_capability_added signal_capability_added () { return m_signal_capability_added; }
    type_signal_capability_removed signal_capability_removed () { return m_signal_capability_removed; }

    Device (const Device&) = delete;
    Device& operator= (const Device&) = delete;

  private:
    type_signal_paired m_signal_paired;
    type_signal_connected m_signal_connected;
    type_signal_disconnected m_signal_disconnected;
    type_signal_message m_signal_message;
    type_signal_capability_added m_signal_capability_added;
    type_signal_capability_removed m_signal_capability_removed;

    std::string m_device_id;
    std::string m_device_name;
    std::string m_device_type;
    uint m_protocol_version;
    uint m_tcp_port;
    Glib::RefPtr<Gio::InetAddress> m_host;
    bool m_is_paired;
    bool m_allowed;
    bool m_is_active;
    std::list<std::string> m_outgoing_capabilities;
    std::list<std::string> m_incoming_capabilities;
    std::vector<std::string> m_capabilities;
    Glib::RefPtr<Gio::TlsCertificate> m_certificate;
    std::string m_certificate_fingerprint;
    bool m_pair_in_progress; // set to true if pair request was sent
    bool m_pair_timeout_source;
    std::unique_ptr<CommunicationChannel> m_channel;
};

} // namespace Conecto

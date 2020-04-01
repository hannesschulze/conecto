/* communication-channel.h
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
#include <sigc++/sigc++.h>
#include <giomm/inetsocketaddress.h>
#include <giomm/socketconnection.h>
#include <giomm/tlsconnection.h>
#include <giomm/dataoutputstream.h>
#include <giomm/datainputstream.h>
#include <giomm/socket.h>
#include <giomm/tlscertificate.h>
#include <giomm/socketsource.h>

namespace Conecto {

/**
 * @brief Device communication channel
 *
 * Automatically handle channel encoding.
 */
class CommunicationChannel {
  public:
    /**
     * Create a new communication channel for a device
     *
     * @param host The IP address
     * @param port The communication port
     */
    CommunicationChannel (const Glib::RefPtr<Gio::InetAddress>& host, uint16_t port);
    ~CommunicationChannel () {}

    /**
     * Try to receive some data from channel
     *
     * @return false if channel was closed, true otherwise
     */
    bool receive ();
    /**
     * Send a packet
     *
     * @param packet instance of Packet
     */
    void send (const NetworkPacket& packet);
    /**
     * Close the communication channel
     */
    void close ();
    /**
     * (Asynchronously) open the communication channel
     *
     * @param cb The callback, where connected is set to true on success
     */
    void open (std::function<void (bool /* connected */)> cb);
    /**
     * Switch channel to TLS mode
     *
     * When TLS was established, `peer_certificate` will store the remote client
     * certificate. If `expected_peer` is null, the peer certificate will be
     * accepted unconditionally during handshake and the caller must eventually
     * decide if the client is to be trusted or not. However, if `expected_peer`
     * was set, the received certificate and expected one will be compared
     * during handshake and connection will be rejected if a mismatch is found.
     *
     * @param expected_peer the peer certificate we are expecting to see
     */
    void secure (const Glib::RefPtr<Gio::TlsCertificate>& expected_peer, std::function<void (bool /* success */)> cb);
    /**
     * Get the peer certificate
     */
    Glib::RefPtr<Gio::TlsCertificate> get_peer_certificate () { return m_peer_certificate; }

    using type_signal_disconnected = sigc::signal<void>;
    /**
     * @param packet The packet received
     */
    using type_signal_packet_received = sigc::signal<void, const NetworkPacket& /* packet */>;
    /**
     * Called when we have been disconnected
     */
    type_signal_disconnected signal_disconnected () { return m_signal_disconnected; }
    /**
     * Called when a new packet has been received through this channel
     */
    type_signal_packet_received signal_packet_received () { return m_signal_packet_received; }

    CommunicationChannel (const CommunicationChannel&) = delete;
    CommunicationChannel& operator= (const CommunicationChannel&) = delete;

  private:
    void replace_streams (const Glib::RefPtr<Gio::InputStream>& input, const Glib::RefPtr<Gio::OutputStream>& output);
    void monitor_events ();
    void unmonitor_events ();
    void handle_packet (const NetworkPacket& packet);
    void fixup_socket ();
    bool on_io_ready (Glib::IOCondition cond);

    type_signal_disconnected    m_signal_disconnected;
    type_signal_packet_received m_signal_packet_received;

    Glib::RefPtr<Gio::InetSocketAddress> m_socket_addr;
    Glib::RefPtr<Gio::SocketConnection>  m_socket_conn;
    Glib::RefPtr<Gio::TlsConnection>     m_tls_conn;
    Glib::RefPtr<Gio::DataOutputStream>  m_data_out;
    Glib::RefPtr<Gio::DataInputStream>   m_data_in;
    Glib::RefPtr<Gio::Socket>            m_socket;
    Glib::RefPtr<Gio::TlsCertificate>    m_peer_certificate;
    Glib::RefPtr<Gio::SocketSource>      m_source;
};

} // namespace Conecto

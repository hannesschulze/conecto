/* communication-channel.cpp
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

#include "communication-channel.h"
#include "exceptions.h"
#include "backend.h"
#include <giomm/socketclient.h>
#include <giomm/unixinputstream.h>
#include <giomm/unixoutputstream.h>
#include <giomm/tlsserverconnection.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

using namespace Conecto;

CommunicationChannel::CommunicationChannel (const Glib::RefPtr<Gio::InetAddress>& host, uint16_t port)
{
    m_socket_addr = Gio::InetSocketAddress::create (host, port);
}

bool
CommunicationChannel::receive ()
{
    std::string data;

    g_assert (m_data_in);

    // Read line up to a newline
    if (!m_data_in->read_upto (data, "\n")) {
        g_warning ("I/O error, connection closed?");
        return false;
    }

    // Expecting \n
    try {
        m_data_in->read_byte ();
    } catch (Glib::Error& err) {
        g_warning ("I/O error, connection closed?");
        return false;
    }

    g_debug ("Received line: %s", data.c_str ());

    try {
        NetworkPacket packet (data);
        handle_packet (packet);
        return true;
    } catch (MalformedPacketException& err) {
        g_warning ("Malformed packet: %s", err.what ());
        // Data was received, hence connection is still alive
        return true;
    }
}

void
CommunicationChannel::send (const NetworkPacket& packet)
{
    std::string to_send = packet.serialize () + "\n";
    g_debug ("Send data: %s", to_send.c_str ());

    g_assert (m_data_out);

    if (!m_data_out->put_string (to_send)) g_warning ("Failed to send message");
}

void
CommunicationChannel::close ()
{
    g_debug ("Closing connection");

    unmonitor_events ();

    try {
        if (m_data_in) m_data_in->close ();
    } catch (Glib::Error& err) {
        g_warning ("Failed to close data input: %s", err.what ().c_str ());
    }
    try {
        if (m_data_out) m_data_out->close ();
    } catch (Glib::Error& err) {
        g_warning ("Failed to close data output: %s", err.what ().c_str ());
    }
    try {
        if (m_tls_conn) m_tls_conn->close ();
    } catch (Glib::Error& err) {
        g_warning ("Failed to close TLS connection: %s", err.what ().c_str ());
    }
    try {
        if (m_socket_conn) m_socket_conn->close ();
    } catch (Glib::Error& err) {
        g_warning ("Failed to close connection: %s", err.what ().c_str ());
    }

    m_data_in.reset ();
    m_data_out.reset ();
    m_socket_conn.reset ();
    m_tls_conn.reset ();
    m_socket.reset ();
    m_peer_certificate.reset ();
}

void
CommunicationChannel::replace_streams (const Glib::RefPtr<Gio::InputStream>&  input,
                                       const Glib::RefPtr<Gio::OutputStream>& output)
{
    if (m_data_out) {
        try {
            m_data_out->close ();
        } catch (Glib::Error& err) {
            g_warning ("Failed to close output stream: %s", err.what ().c_str ());
        }
    }
    m_data_out = Gio::DataOutputStream::create (output);

    if (m_data_in) {
        try {
            m_data_in->close ();
        } catch (Glib::Error& err) {
            g_warning ("Failed to close input stream: %s", err.what ().c_str ());
        }
    }
    m_data_in = Gio::DataInputStream::create (input);

    // Messages end with \n\n
    m_data_in->set_newline_type (Gio::DATA_STREAM_NEWLINE_TYPE_LF);
}

void
CommunicationChannel::monitor_events ()
{
    m_source = m_socket->create_source (Glib::IO_IN);
    m_source->connect (sigc::mem_fun (*this, &CommunicationChannel::on_io_ready));
    // Attach source
    m_source->attach ();
}

void
CommunicationChannel::unmonitor_events ()
{
    if (m_source) {
        m_source->destroy ();
        m_source.reset ();
    }
}

bool
CommunicationChannel::on_io_ready (Glib::IOCondition cond)
{
    g_debug ("check for IO");
    bool res = receive ();

    if (!res)
        // Disconnected
        m_signal_disconnected.emit ();

    return res;
}

void
CommunicationChannel::handle_packet (const NetworkPacket& packet)
{
    if (packet.get_type () == PacketTypes::TYPE_ENCRYPTED)
        g_warning (
                "Received packet with explicit encryption, this usually indicates a protocol version < 6 type packet, "
                "such packets are no longer supported, dropping..");
    else
        // Signal that we got a packet
        m_signal_packet_received.emit (packet);
}

void
CommunicationChannel::fixup_socket ()
{
    // Set keepalive counters on socket
    int option = 10;
    setsockopt (m_socket->get_fd (), IPPROTO_TCP, TCP_KEEPIDLE, &option, sizeof (int));
    option = 5;
    setsockopt (m_socket->get_fd (), IPPROTO_TCP, TCP_KEEPINTVL, &option, sizeof (int));
    option = 3;
    setsockopt (m_socket->get_fd (), IPPROTO_TCP, TCP_KEEPCNT, &option, sizeof (int));

    // Enable keepalive
    m_socket->set_keepalive (true);
    m_socket->set_timeout (0);
}

void
CommunicationChannel::open (std::function<void (bool)> cb)
{
    g_assert (m_socket_addr);
    g_debug ("Connecting to %s:%u", m_socket_addr->get_address ()->to_string ().c_str (), m_socket_addr->get_port ());

    auto client = Gio::SocketClient::create ();
    client->connect_async (m_socket_addr, [this, cb, client] (Glib::RefPtr<Gio::AsyncResult>& res) {
        Glib::RefPtr<Gio::SocketConnection> conn;
        try {
            conn = client->connect_finish (res);
        } catch (Glib::Error& err) {
            g_warning ("Failed to connect: %s", err.what ().c_str ());
            // return disconnected
            cb (false);
            return;
        }
        g_debug ("Connected");

        m_socket = conn->get_socket ();

        // Fixup socket keepalive
        fixup_socket ();

        m_socket_conn = conn;

        // Input/output streams will close underlying base stream when .close () is called on them, make sure
        // we pass Unix+Stream with which can skip closing the socket
        replace_streams (Gio::UnixInputStream::create (m_socket->get_fd (), false),
                         Gio::UnixOutputStream::create (m_socket->get_fd (), false));

        // Start monitoring socket events
        monitor_events ();

        cb (true);
    });
}

void
CommunicationChannel::secure (const Glib::RefPtr<Gio::TlsCertificate>& expected_peer, std::function<void (bool)> cb)
{
    g_assert (m_socket_conn);

    // Stop monitoring socket events
    unmonitor_events ();

    auto cert = Backend::get_instance ().get_certificate ();

    // Make TLS connection
    g_debug ("Creating TLS server connection");
    GError* err = nullptr;
    auto    stream = g_tls_server_connection_new (G_IO_STREAM (m_socket_conn->gobj ()), cert->gobj (), &err);
    g_assert (err == nullptr);
    auto tls_server = std::make_shared<Gio::TlsServerConnection> (G_TLS_SERVER_CONNECTION (stream));
    tls_server->property_authentication_mode ().set_value (Gio::TLS_AUTHENTICATION_REQUESTED);
    tls_server->signal_accept_certificate ().connect (
            [this, expected_peer] (const Glib::RefPtr<const Gio::TlsCertificate>& peer_cert,
                                   Gio::TlsCertificateFlags                       errors) {
                g_info ("Accept certificate, flags: 0x%x", errors);
                g_info ("Certificate:\n%s\n---", peer_cert->property_certificate_pem ().get_value ().c_str ());
                if (expected_peer) {
                    if (expected_peer->is_same (peer_cert)) {
                        return true;
                    } else {
                        g_warning ("Rejecting handshake, peer certificate mismatch, got:\n%s\n---",
                                   peer_cert->property_certificate_pem ().get_value ().c_str ());
                        return false;
                    }
                }
                return true;
            });

    // Wrap with TLS
    g_info ("Attempt TLS handshake");
    tls_server->handshake_async ([this, cb, tls_server] (Glib::RefPtr<Gio::AsyncResult>& res) {
        try {
            bool success = tls_server->handshake_finish (res);
            if (success) {
                g_info ("TLS handshake successful");
                m_peer_certificate = tls_server->get_peer_certificate ();
            } else {
                g_warning ("TLS handshake unsuccessful");
                cb (false);
                return;
            }
        } catch (Glib::Error& err) {
            g_warning ("TLS handshake failed: %s", err.what ().c_str ());
            cb (false);
            return;
        }

        m_tls_conn = tls_server;
        // Data will now pass through the TLS stream wrapper
        replace_streams (m_tls_conn->get_input_stream (), m_tls_conn->get_output_stream ());

        // Monitor socket events
        monitor_events ();
        cb (true);
    });
}
/* discovery.cpp
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

#include "discovery.h"
#include "exceptions.h"
#include "network-packet.h"
#include <giomm/inetsocketaddress.h>
#include <giomm/socketsource.h>
#include <gio/gio.h>

using namespace Conecto;

namespace {

constexpr guint16 SOCKET_PORT = 1716;

} // namespace

Discovery::Discovery ()
{
}

Discovery::~Discovery ()
{
    if (m_socket) m_socket->close ();
}

void
Discovery::listen ()
{
    m_socket = Gio::Socket::create (Gio::SOCKET_FAMILY_IPV4, Gio::SOCKET_TYPE_DATAGRAM, Gio::SOCKET_PROTOCOL_UDP);
    auto socket_addr =
            Gio::InetSocketAddress::create (Gio::InetAddress::create_any (Gio::SOCKET_FAMILY_IPV4), SOCKET_PORT);

    try {
        m_socket->bind (socket_addr, false);
    } catch (Gio::Error& err) {
        m_socket->close ();
        m_socket.reset ();
        throw BindSocketException (err.code (), err.what ());
    }

    Glib::RefPtr<Gio::SocketSource> source = m_socket->create_source (Glib::IO_IN);
    source->connect (sigc::mem_fun (*this, &Discovery::on_packet));
    source->attach (Glib::MainContext::get_default ());
}

bool
Discovery::on_packet (Glib::IOCondition condition)
{
    char                                 buffer[4096];
    Glib::RefPtr<Gio::SocketAddress>     socket_addr;
    Glib::RefPtr<Gio::InetSocketAddress> inet_socket_addr;

    try {
        gssize read = m_socket->receive_from (socket_addr, buffer, 4096);
        inet_socket_addr = Glib::RefPtr<Gio::InetSocketAddress>::cast_dynamic (socket_addr);
        g_debug ("Received %zd bytes from %s:%u", read, inet_socket_addr->get_address ()->to_string ().c_str (),
                 inet_socket_addr->get_port ());
    } catch (Gio::Error& err) {
        g_warning ("Failed to receive packet: %s", err.what ().c_str ());
        return true;
    }

    parse_packet (std::string (buffer), inet_socket_addr->get_address ());

    return true;
}

void
Discovery::parse_packet (std::string&& data, const Glib::RefPtr<Gio::InetAddress>& host)
{
    try {
        NetworkPacket packet (data);

        // Expecting an identity packet
        if (packet.get_type () != PacketTypes::TYPE_IDENTITY) {
            g_warning ("Unexpected packet type %s from device %s", packet.get_type ().c_str (),
                       host->to_string ().c_str ());
            return;
        }

        auto device = std::make_shared<Device> (packet, host);
        m_signal_device_found.emit (device);
    } catch (MalformedPacketException& err) {
        g_warning ("Malformed packet: %s", err.what ());
    } catch (MalformedIdentityException& err) {
        g_warning ("Malformed identity packet: %s", err.what ());
    }
}
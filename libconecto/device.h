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
#include "communication-channel.h"
#include "abstract-packet-handler.h"
#include <giomm/inetaddress.h>
#include <giomm/tlscertificate.h>
#include <glibmm/keyfile.h>

namespace Conecto {

/**
 * @brief A class representing a device, managing its communication channel and capabilities
 */
class Device : public std::enable_shared_from_this<Device> {
  public:
    /**
     * Constructs a new Device wrapper based on an identity packet
     *
     * @param packet An identity packet
     * @param host Source host that the packet came from
     * @throw MalformedPacketException
     */
    static std::shared_ptr<Device> create_from_packet (const NetworkPacket&                  packet,
                                                       const Glib::RefPtr<Gio::InetAddress>& host);
    /**
     * Constructs a new Device wrapper based on data read from device cache file.
     *
     * @param cache device cache file
     * @param name device name
     * @throw Glib::KeyFileError, InvalidIPAddressException
     */
    static std::shared_ptr<Device> create_from_cache (Glib::KeyFile& cache, const std::string& name);
    ~Device () {}

    /**
     * Dump device information to cache
     *
     * @param cache [ref] Device cache
     * @param name Group name
     */
    void to_cache (Glib::KeyFile& cache, const std::string& name) const noexcept;
    /**
     * Update information/state of this device using data from @other_dev. This
     * may happen in case when a discovery packet was received, or a device got
     * connected. In such case, a this device (which was likely created from
     * cached data) needs to be updated.
     *
     * As a side effect, updating capabilities will emit @signal_capability_added
     * and @signal_capability_removed
     *
     * @param device The other device
     */
    void update (const Device& device) noexcept;

    /** @brief Get the device's id */
    const std::string& get_device_id () const noexcept;
    /** @brief Get the device's name */
    const std::string& get_device_name () const noexcept;
    /** @brief Get the device's type */
    const std::string& get_device_type () const noexcept;
    /** @brief Get the protocol version used with the device */
    const uint& get_protocol_version () const noexcept;
    /** @brief Get the port used for communicating with the device */
    const uint& get_tcp_port () const noexcept;
    /** @brief Get the address used for communicating with the device */
    Glib::RefPtr<Gio::InetAddress> get_host () const noexcept;
    /** @brief true if the device is currently paired */
    const bool& get_is_paired () const noexcept;
    /** @brief A flag used for checking if a specific device is allowed */
    const bool& get_allowed () const noexcept;
    /** @brief true if the device is currently active */
    const bool& get_is_active () const noexcept;
    /** @brief Get a list of outgoing capabilities */
    const std::list<std::string>& get_outgoing_capabilities () const noexcept;
    /** @brief Get a list of incoming capabilities */
    const std::list<std::string>& get_incoming_capabilities () const noexcept;
    /** @brief Get the certificate used for encryption */
    Glib::RefPtr<Gio::TlsCertificate> get_certificate () const noexcept;
    /** @brief Get the PEM encoded certificate used for encryption */
    std::string get_certificate_pem () const noexcept;
    /** @brief Get the certificate's SHA1 encoded fingerprint */
    const std::string& get_certificate_fingerprint () const noexcept;

    /** @brief A flag used for checking if a specific device is allowed */
    void set_allowed (bool allowed) noexcept;
    /** @brief Update the certificate used for encryption */
    void set_certificate (Glib::RefPtr<Gio::TlsCertificate> certificate) noexcept;

    /**
     * Get a short string representing this device
     */
    std::string to_string () const noexcept;
    /**
     * Get a short string representing this device
     */
    std::string to_unique_string () const noexcept;

    /**
     * Internally changes pair requests state tracking.
     *
     * @param expect_response set to true if expecting a response
     */
    void pair (bool expect_response) noexcept;
    /**
     * Trigger pairing or call @p handle_pair if already paired.
     */
    void maybe_pair () noexcept;
    /**
     * Activate device. Emits @p signal_connected after successfuly opening a connection.
     */
    void activate () noexcept;
    /**
     * Deactivate device
     */
    void deactivate () noexcept;
    /**
     * Send a packet
     */
    void send (const NetworkPacket& packet) noexcept;

    /**
     * Returns true if there is a handler of capability @cap registed for this device.
     *
     * @param capability The capability name, e.g. kdeconnect.notification
     * @return true if there is a handler of capability @cap registed for this device
     */
    bool has_capability_handler (const std::string& capability) const noexcept;
    /**
     * Register this device in the capability handler and add it to the handlers-list
     *
     * @param capability Capability, e.g. kdeconnect.notification
     * @param handler Packet handler
     * @throw PacketHandlerAlreadyRegisteredException
     */
    void register_capability_handler (const std::string&                            capability,
                                      const std::shared_ptr<AbstractPacketHandler>& handler);
    /**
     * Unregister this device from the capability handler and remove it from the handlers-list
     *
     * @param capability Capability, e.g. kdeconnect.notification
     * @throw PacketHandlerNotRegisteredException
     */
    void unregister_capability_handler (const std::string& capability);

    /**
     * @param pair true if the device was successfully paired
     */
    using type_signal_paired = sigc::signal<void, bool /* pair */>;
    using type_signal_connected = sigc::signal<void>;
    using type_signal_disconnected = sigc::signal<void>;
    /**
     * @param message The packet received
     */
    using type_signal_message = sigc::signal<void, const NetworkPacket& /* message */>;
    /**
     * @param capability The new capability's name
     */
    using type_signal_capability_added = sigc::signal<void, const std::string& /* capability */>;
    /**
     * @param capability The removed capability's name
     */
    using type_signal_capability_removed = sigc::signal<void, const std::string& /* capability */>;
    /**
     * Emitted when the device has been paired
     */
    type_signal_paired signal_paired () { return m_signal_paired; }
    /**
     * Emitted when we are connected to the device
     */
    type_signal_connected signal_connected () { return m_signal_connected; }
    /**
     * Emitted when we are disconnected from the device
     */
    type_signal_disconnected signal_disconnected () { return m_signal_disconnected; }
    /**
     * Emitted when we received a new packet from the device
     */
    type_signal_message signal_message () { return m_signal_message; }
    /**
     * Emitted when a capability has been added to this device (most likely read from the cache)
     */
    type_signal_capability_added signal_capability_added () { return m_signal_capability_added; }
    /**
     * Emitted when a capability has been removed from this device (most likely on @p update call)
     */
    type_signal_capability_removed signal_capability_removed () { return m_signal_capability_removed; }

    Device (const Device&) = delete;
    Device& operator= (const Device&) = delete;

  private:
    /**
     * Default constructor
     */
    Device ();

    void greet (std::function<void ()> cb) noexcept;
    bool on_pair_timeout ();
    void on_packet_received (const NetworkPacket& packet);
    void handle_pair (bool pair) noexcept;
    void close_and_cleanup () noexcept;
    void channel_closed_cleanup () noexcept;
    /**
     * Merge and update existing outgoing_capabilities and incoming_capabilities.
     * Returns lists of added and removed capabilities.
     *
     * @param added [out] Capabilities that were added
     * @param removed [out] Capabilities that were removed
     */
    void merge_capabilities (std::vector<std::string>& added, std::vector<std::string>& removed) noexcept;
    void update_certificate (const Glib::RefPtr<Gio::TlsCertificate>& certificate) noexcept;

    type_signal_paired             m_signal_paired;
    type_signal_connected          m_signal_connected;
    type_signal_disconnected       m_signal_disconnected;
    type_signal_message            m_signal_message;
    type_signal_capability_added   m_signal_capability_added;
    type_signal_capability_removed m_signal_capability_removed;

    std::string                           m_device_id;
    std::string                           m_device_name;
    std::string                           m_device_type;
    uint                                  m_protocol_version;
    uint                                  m_tcp_port;
    Glib::RefPtr<Gio::InetAddress>        m_host;
    bool                                  m_is_paired;
    bool                                  m_allowed;
    bool                                  m_is_active;
    std::list<std::string>                m_outgoing_capabilities;
    std::list<std::string>                m_incoming_capabilities;
    std::list<std::string>                m_capabilities;
    Glib::RefPtr<Gio::TlsCertificate>     m_certificate;
    std::string                           m_certificate_fingerprint;
    bool                                  m_pair_in_progress; // set to true if pair request was sent
    sigc::connection                      m_pair_timeout_connection;
    std::unique_ptr<CommunicationChannel> m_channel;

    std::map<std::string, std::shared_ptr<AbstractPacketHandler>> m_handlers;
};

} // namespace Conecto

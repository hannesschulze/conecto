/* backend.h
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

#include "discovery.h"
#include "device.h"
#include "config-file.h"
#include <map>
#include <string>
#include <giomm/tlscertificate.h>

namespace Conecto {

/**
 * @brief The main singleton used for managing available devices, discovering new devices and managing plugins
 */
class Backend {
  public:
    /**
     * @throw (on first call) GnuTLSInitializationError, PEMWriteError, InvalidCertificateException
     */
    static Backend& get_instance ();
    ~Backend ();

    /**
     * Start listening for new devices
     *
     * @throw BindSocketException
     */
    void listen ();
    /**
     * Load existing devices from the cache
     */
    void load_from_cache () noexcept;

    /**
     * A register a new plugin (capability/packet handler). If the capability already exists, it will be overridden
     *
     * @param handler The packet handler for this capability
     */
    void register_plugin (const std::shared_ptr<AbstractPacketHandler>& handler) noexcept;
    /**
     * Get the packet handler for a capability (may be empty)
     *
     * @param capability The capability name (see @p register_plugin)
     * @return The handler interface (or nullptr)
     */
    std::shared_ptr<AbstractPacketHandler> get_plugin (const std::string& capability) const noexcept;

    /**
     * Get a reference to the @p ConfigFile that is automatically loaded by the backend
     *
     * @return A reference to the @p ConfigFile in use
     */
    ConfigFile& get_config () noexcept;
    /**
     * Get a reference to the @p ConfigFile that is automatically loaded by the backend
     *
     * @return A const reference to the @p ConfigFile in use
     */
    const ConfigFile& get_config () const noexcept;

    /**
     * Get the certificate used for encryption
     *
     * @return The certificate
     * @see Crypt
     */
    Glib::RefPtr<Gio::TlsCertificate> get_certificate () const noexcept;
    /**
     * Get a list of packet handler interface names available from plugins
     *
     * @return A doubly linked list containing the available interface names
     */
    std::list<std::string> get_handler_interfaces () const noexcept;

    /**
     * Get the storage location (used for storing the key and certificate)
     */
    static std::string get_storage_dir () noexcept;
    /**
     * Get the launcher location (used for storing desktop files for displaying popovers)
     */
    static std::string get_launcher_dir () noexcept;
    /**
     * Get the config location (used as a base path for loading the @p ConfigFile)
     */
    static std::string get_config_dir () noexcept;
    /**
     * Get the cache location (used for caching devices over multiple sessions)
     */
    static std::string get_cache_dir () noexcept;
    /**
     * Create user dirs if they don't already exist
     */
    static void init_user_dirs ();

    /**
     * @param device The new device
     */
    using type_signal_found_new_device = sigc::signal<void, const std::shared_ptr<Device>& /* device */>;
    /**
     * @param device The device the capability was added to
     * @param capability The new capability's name
     * @param handler The new capability's handler interface
     */
    using type_signal_device_capability_added =
            sigc::signal<void, const std::shared_ptr<Device>& /* device */, const std::string& /* capability */,
                         const std::shared_ptr<AbstractPacketHandler>& /* handler */>;
    /**
     * Called when a new device was found (not called if the device was already in the cache)
     */
    type_signal_found_new_device signal_found_new_device () { return m_signal_found_new_device; }
    /**
     * Called when a new capability has been added to a device (most likely read from the cache)
     */
    type_signal_device_capability_added signal_device_capability_added () { return m_signal_device_capability_added; }

    Backend (const Backend&) = delete;
    Backend& operator= (const Backend&) = delete;

  protected:
    Backend ();

  private:
    struct DeviceEntry {
        DeviceEntry (const std::shared_ptr<Device>& dev)
            : device (dev)
        {
        }
        DeviceEntry (const DeviceEntry&) = delete;
        DeviceEntry& operator= (const DeviceEntry&) = delete;

        sigc::connection        paired_conn;
        sigc::connection        disconnected_conn;
        std::shared_ptr<Device> device;
    };

    void        on_new_device (std::shared_ptr<Device> device);
    void        on_capability_added (const std::string& cap, const std::shared_ptr<Device>& device);
    void        on_capability_removed (const std::string& cap, const std::shared_ptr<Device>& device);
    void        activate_device (const std::shared_ptr<DeviceEntry>& entry);
    std::string get_cache_file () const; // Path to devices cache file
    void        update_cache () noexcept;

    std::map<std::string, std::shared_ptr<DeviceEntry>> m_devices;

    Discovery                         m_discovery;
    std::unique_ptr<ConfigFile>       m_config;
    Glib::RefPtr<Gio::TlsCertificate> m_certificate;

    std::map<std::string, std::shared_ptr<AbstractPacketHandler>> m_plugins;

    type_signal_found_new_device        m_signal_found_new_device;
    type_signal_device_capability_added m_signal_device_capability_added;
};

} // namespace Conecto
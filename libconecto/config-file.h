/* config-file.h
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
#include <vector>
#include <glibmm/keyfile.h>

namespace Conecto {

// forward declarations
class Device;

/**
 * @brief A config file stores a list of devices and their options (e.g. whether they are allowed)
 */
class ConfigFile {
  public:
    /**
     * Read the config file from @p get_search_dirs
     *
     * @param base_config_dir The preferred config dir
     */
    ConfigFile (const std::string& base_config_dir);
    ~ConfigFile () {}

    /**
     * Get the actual path the config has been read from
     *
     * @return The path
     */
    const std::string& get_path () const noexcept;
    /**
     * Get the filename to use for the config file
     *
     * @return A constant
     */
    static std::string get_file_name () noexcept;
    /**
     * Dump the configuration to a file
     *
     * @param path The path where the config should be saved
     */
    void dump_to_file (const std::string& path);
    /**
     * Get a device's display name from the configuration. If there is no entry for the specified device,
     * this will return @p name
     *
     * @param device The device
     */
    Glib::ustring get_display_name (const Device& device) const;
    /**
     * Set a device's name in the config file and save the file
     *
     * @param device The device
     * @param display The display name
     */
    void set_display_name (const std::shared_ptr<Device>& device, const Glib::ustring& display);
    /**
     * Check if a device is starred. If there is no entry for the specified device, this will return @p false
     *
     * @param device The device
     */
    bool get_device_starred (const Device& device) const;
    /**
     * Set if a device is starred and save the config file
     *
     * @param device The device
     * @param starred true if the device has been starred
     */
    void set_device_starred (const std::shared_ptr<Device>& device, bool starred);

    /**
     * Get a list of search dirs
     *
     * @param primary_dir The preferred search path (will be put in index 0)
     */
    static std::vector<std::string> get_search_dirs (const std::string& primary_dir) noexcept;

    /**
     * @param device The device updated in the configuration
     */
    using type_signal_device_changed = sigc::signal<void, const std::shared_ptr<Device>& /* device */>;
    /**
     * Called when a new device configuration has been changed
     */
    type_signal_device_changed signal_device_changed () { return m_signal_device_changed; }

    ConfigFile (const ConfigFile&) = delete;
    ConfigFile& operator= (const ConfigFile&) = delete;

  private:
    Glib::KeyFile m_keyfile;
    std::string   m_path;

    type_signal_device_changed m_signal_device_changed;
};

} // namespace Conecto
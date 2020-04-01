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
     * Check if a device is specified as allowed in the configuration
     *
     * @param name The device's name
     * @param type The device's type
     */
    bool get_device_allowed (const std::string& name, const std::string& type);

    /**
     * Get a list of search dirs
     *
     * @param primary_dir The preferred search path (will be put in index 0)
     */
    static std::vector<std::string> get_search_dirs (const std::string& primary_dir) noexcept;

    ConfigFile (const ConfigFile&) = delete;
    ConfigFile& operator= (const ConfigFile&) = delete;

  private:
    Glib::KeyFile m_keyfile;
    std::string   m_path;
};

} // namespace Conecto
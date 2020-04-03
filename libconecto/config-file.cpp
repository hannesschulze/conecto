/* config-file.cpp
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

#include "config-file.h"
#include <glibmm/miscutils.h>
#include <glibmm/fileutils.h>
#include <iostream>

using namespace Conecto;

namespace {

constexpr char CONFIG_FILE[] = "conecto.conf";

} // namespace

ConfigFile::ConfigFile (const std::string& base_config_dir)
{
    auto        dirs = get_search_dirs (base_config_dir);
    std::string full_path;

    for (const auto& dir : dirs) g_debug ("Config search dir: %s", dir.c_str ());

    try {
        bool found = m_keyfile.load_from_dirs (CONFIG_FILE, dirs, full_path, Glib::KEY_FILE_KEEP_COMMENTS);

        m_path = full_path;
        if (!found) g_critical ("Configuration file %s not found", CONFIG_FILE);
        g_message ("Loaded configuration from %s", full_path.c_str ());
    } catch (Glib::KeyFileError& err) {
        g_critical ("Failed to parse configuration file: %s", err.what ().c_str ());
        m_keyfile = Glib::KeyFile ();
    } catch (Glib::FileError& err) {
        g_critical ("Failed to read configuration file: %s", err.what ().c_str ());
        m_keyfile = Glib::KeyFile ();
    }
}

const std::string&
ConfigFile::get_path () const noexcept
{
    return m_path;
}

std::string
ConfigFile::get_file_name () noexcept
{
    return CONFIG_FILE;
}

void
ConfigFile::dump_to_file (const std::string& path)
{
    std::string data = m_keyfile.to_data ();
    try {
        Glib::file_set_contents (path, data);
    } catch (Glib::FileError& err) {
        g_critical ("Failed to save configuration file to %s: %s", path.c_str (), err.what ().c_str ());
    }
}

bool
ConfigFile::get_device_allowed (const std::string& name, const std::string& type)
{
    try {
        std::vector<std::string> devices = m_keyfile.get_string_list ("main", "devices");

        for (const auto& dev : devices) {
            if (m_keyfile.has_group (dev) == false) {
                g_critical ("No group in keyfile: %s", dev.c_str ());
                continue;
            }

            if (m_keyfile.get_string (dev, "name") == name && m_keyfile.get_string (dev, "type") == type &&
                m_keyfile.get_boolean (dev, "allowed"))
                return true;
        }
    } catch (Glib::KeyFileError& err) {
        g_critical ("Failed to read entries from configuration file: %s", err.what ().c_str ());
    }
    return false;
}

std::vector<std::string>
ConfigFile::get_search_dirs (const std::string& primary_dir) noexcept
{
    std::vector<std::string> dirs = { primary_dir };

    std::vector<std::string> sysdirs = Glib::get_system_data_dirs ();
    for (const auto& dir : sysdirs) {
        std::vector<std::string> parts = { dir, "conecto" };
        dirs.push_back (Glib::build_path (G_DIR_SEPARATOR_S, parts));
    }

    return dirs;
}
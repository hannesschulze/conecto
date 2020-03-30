/* backend.cpp
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

#include "backend.h"
#include "crypt.h"
#include "exceptions.h"
#include <glibmm/miscutils.h>
#include <glibmm/fileutils.h>
#include <giomm/file.h>

using namespace Conecto;

namespace {

constexpr char APP_NAME[] = "conecto";

} // namespace

Backend&
Backend::get_instance ()
{
    static Backend instance;
    return instance;
}

Backend::Backend ()
{
    init_user_dirs ();

    auto key_file  = Gio::File::create_for_path (Glib::build_filename (get_storage_dir (), "private.pem"));
    auto cert_file = Gio::File::create_for_path (Glib::build_filename (get_storage_dir (), "certificate.pem"));

    if (!key_file->query_exists () || !cert_file->query_exists ()) {
        std::string host_name = Glib::get_host_name ();
        std::string user = Glib::get_user_name ();
        Crypt::generate_key_cert (key_file->get_path (), cert_file->get_path (), user + "@" + host_name);
    }

    GError* err;
    g_tls_certificate_new_from_files (cert_file->get_path ().c_str (), key_file->get_path ().c_str (), &err);
    if (err) {
        g_error_free (err);
        throw InvalidCertificateException ("Failed to load certificate or key");
    }

    // Set up config
    std::string user_config_path = get_config_dir () + "/" + ConfigFile::get_file_name ();
    m_config = std::make_unique<ConfigFile> (user_config_path);

    // Write configuration to user config file if not present
    if (m_config->get_path () != user_config_path)
        m_config->dump_to_file (user_config_path);

    // Listen to new devices
    m_discovery.signal_device_found ().connect (sigc::mem_fun (+this, &Backend::on_new_device));
}

ConfigFile&
Backend::get_config () noexcept
{
    return *m_config;
}

const ConfigFile&
Backend::get_config () const noexcept
{
    return *m_config;
}

void
Backend::listen ()
{
    m_discovery.listen ();
}

Glib::RefPtr<Gio::TlsCertificate>
Backend::get_certificate () const noexcept
{
    return m_certificate;
}

std::list<std::string>
Backend::get_handler_interfaces () const noexcept
{
    // TODO
    return {};
}

std::string
Backend::get_storage_dir () noexcept
{
    return Glib::build_filename (Glib::get_user_data_dir (), APP_NAME);
}

std::string
Backend::get_config_dir () noexcept
{
    return Glib::build_filename (Glib::get_user_config_dir (), APP_NAME);
}

void
Backend::init_user_dirs ()
{
    g_mkdir_with_parents (get_storage_dir ().c_str (), 0700);
    g_mkdir_with_parents (get_config_dir ().c_str (), 0700);
}

void
Backend::on_new_device (std::shared_ptr<Device> device)
{
    bool is_new = false;
    std::string unique = device->to_unique_string ();

    if (m_devices.find (unique) == m_devices.end ()) {
        g_debug ("Adding new device with key: %s", unique.c_str ());
        m_devices.insert ({ unique, device });
        is_new = true;
    } else {
        g_debug ("Device %s already present", unique.c_str ());
        m_devices.at (unique).swap (device);
    }

    device = m_devices.at (unique);

    // Notify everyone that a new device appeared
    if (is_new) {
        m_signal_found_new_device.emit (*device);

        device->signal_capability_added ().connect (sigc::bind (sigc::mem_fun (*this, &Backend::on_capability_added), device));
        device->signal_capability_removed ().connect (sigc::bind (sigc::mem_fun (*this, &Backend::on_capability_removed), device));
    }

    g_debug ("Allowed? %s", device->get_allowed () ? "true" : "false");

    // Check if the device is whitelisted in configuration
    if (!device->get_allowed () && get_allowed_in_config (*device))
        device->set_allowed (true);
    
    // Update device cache
    // TODO: update_cache ();

    if (device->get_allowed ())
        // Device is allowed
        activate_device (*device);
    else
        g_warning ("Skipping device %s activation, not allowed", device->to_string ().c_str ());
}

bool
Backend::get_allowed_in_config (const Device& device) const
{
    if (device.get_allowed ())
        return true;

    return m_config->get_device_allowed (device.get_device_name (), device.get_device_type ());
}

void
Backend::activate_device (Device& device)
{
    g_info ("Activating device %s", device.to_string ().c_str ());

    if (!device.get_is_active ()) {
        sigc::connection conn = device.signal_paired ().connect ([this, &device](bool success) {
            // TODO: update_cache ()
            if (!success)
                // Deactivate if needed
                device.deactivate ();
        });
        device.signal_disconnected ().connect ([this, &device, conn]() {
            g_debug ("Device %s got disconnected", device.to_string ().c_str ());
        });
    }
}

void
Backend::on_capability_added (const std::string& cap, const std::shared_ptr<Device>& device)
{
    // TODO
}

void
Backend::on_capability_removed (const std::string& cap, const std::shared_ptr<Device>& device)
{
    // TODO
}
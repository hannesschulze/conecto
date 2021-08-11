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
#include "constants.h"
#include "exceptions.h"
#include <glibmm/miscutils.h>
#include <glibmm/fileutils.h>
#include <giomm/file.h>
#include <libnotify/notify.h>

using namespace Conecto;

namespace {

constexpr char APP_NAME[] = "conecto";
constexpr char DEVICES_CACHE_FILE[] = "devices";

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

    auto key_file = Gio::File::create_for_path (Glib::build_filename (get_storage_dir (), "private.pem"));
    auto cert_file = Gio::File::create_for_path (Glib::build_filename (get_storage_dir (), "certificate.pem"));

    if (!key_file->query_exists () || !cert_file->query_exists ()) {
        std::string host_name = g_get_host_name ();
        std::string user = Glib::get_user_name ();
        Crypt::generate_key_cert (key_file->get_path (), cert_file->get_path (), user + "@" + host_name);
    }

    GError*          err = nullptr;
    GTlsCertificate* cert =
            g_tls_certificate_new_from_files (cert_file->get_path ().c_str (), key_file->get_path ().c_str (), &err);
    if (err) {
        g_error_free (err);
        throw InvalidCertificateException ("Failed to load certificate or key");
    }
    if (!cert) throw InvalidCertificateException ("Failed to load certificate or key");
    m_certificate = Glib::wrap (cert);

    // Set up config
    m_config = std::make_unique<ConfigFile> (get_config_dir ());

    // Listen to new devices
    m_discovery.signal_device_found ().connect (sigc::mem_fun (+this, &Backend::on_new_device));

    // Set up notifications
    notify_init (Constants::APP_ID.c_str ());
}

Backend::~Backend ()
{
    notify_uninit ();
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
    std::list<std::string> res;
    for (const auto& plugin : m_plugins) res.push_back (plugin.first);
    return res;
}

std::string
Backend::get_storage_dir () noexcept
{
    return Glib::build_filename (Glib::get_user_data_dir (), APP_NAME);
}

std::string
Backend::get_launcher_dir () noexcept
{
    return Glib::build_filename (get_storage_dir (), "launchers");
}

std::string
Backend::get_config_dir () noexcept
{
    return Glib::build_filename (Glib::get_user_config_dir (), APP_NAME);
}

std::string
Backend::get_cache_dir () noexcept
{
    return Glib::build_filename (Glib::get_user_cache_dir (), APP_NAME);
}

void
Backend::init_user_dirs ()
{
    g_mkdir_with_parents (get_storage_dir ().c_str (), 0700);
    g_mkdir_with_parents (get_launcher_dir ().c_str (), 0700);
    g_mkdir_with_parents (get_config_dir ().c_str (), 0700);
}

void
Backend::on_new_device (std::shared_ptr<Device> new_device)
{
    bool        is_new = false;
    std::string unique = new_device->to_unique_string ();

    if (m_devices.find (unique) == m_devices.end ()) {
        g_debug ("Adding new device with key: %s", unique.c_str ());
        m_devices.insert ({ unique, std::make_shared<DeviceEntry> (new_device) });
        is_new = true;
    } else {
        g_debug ("Device %s already present", unique.c_str ());
    }

    auto entry = m_devices.at (unique);
    auto device = entry->device;

    // Notify everyone that a new device appeared
    if (is_new) {
        m_signal_found_new_device.emit (device);

        device->signal_capability_added ().connect (
                sigc::bind (sigc::mem_fun (*this, &Backend::on_capability_added), device));
        device->signal_capability_removed ().connect (
                sigc::bind (sigc::mem_fun (*this, &Backend::on_capability_removed), device));
    }

    device->update (*new_device);

    // Update device cache
    update_cache ();

    // Establish a connection
    activate_device (entry);
}

void
Backend::activate_device (const std::shared_ptr<DeviceEntry>& device)
{
    g_info ("Activating device %s", device->device->to_string ().c_str ());

    if (!device->device->get_is_active ()) {
        device->paired_conn =
                device->device->signal_paired ().connect ([this, device] (bool success) { update_cache (); });
        device->disconnected_conn = device->device->signal_disconnected ().connect ([device] () {
            g_debug ("Device %s got disconnected", device->device->to_string ().c_str ());

            device->disconnected_conn.disconnect ();
        });
        device->device->activate ();
    }
}

void
Backend::on_capability_added (const std::string& cap, const std::shared_ptr<Device>& device)
{
    g_info ("Capability %s added to device %s", cap.c_str (), device->to_string ().c_str ());

    if (device->has_capability_handler (cap)) return;

    if (m_plugins.find (cap) != m_plugins.end ()) {
        auto handler = m_plugins.at (cap);
        device->register_capability_handler (cap, handler);
        m_signal_device_capability_added.emit (device, cap, handler);
    } else {
        g_debug ("No handler for capability %s", cap.c_str ());
    }
}

void
Backend::on_capability_removed (const std::string& cap, const std::shared_ptr<Device>& device)
{
    g_info ("Capability %s removed from device %s", cap.c_str (), device->to_string ().c_str ());
}

std::string
Backend::get_cache_file () const
{
    std::string cache_file = Glib::build_filename (get_cache_dir (), DEVICES_CACHE_FILE);
    g_debug ("Cache file: %s", cache_file.c_str ());

    // Make sure that the cache dir exists
    g_mkdir_with_parents (get_cache_dir ().c_str (), 0700);

    return cache_file;
}

void
Backend::load_from_cache () noexcept
{
    std::string cache_file = get_cache_file ();

    g_debug ("Trying to load devices from cache");

    Glib::KeyFile keyfile;
    try {
        keyfile.load_from_file (cache_file);

        std::vector<std::string> groups = keyfile.get_groups ();
        for (const auto& group : groups) {
            try {
                on_new_device (Device::create_from_cache (keyfile, group));
            } catch (...) {
                // ignore
            }
        }
    } catch (Glib::Error& err) {
        g_debug ("Couldn't load cache file: %s", err.what ().c_str ());
    }
}

void
Backend::update_cache () noexcept
{
    if (m_devices.empty ()) return;

    Glib::KeyFile kf;

    for (const auto& dev : m_devices) dev.second->device->to_cache (kf, dev.second->device->get_device_name ());

    try {
        g_debug ("Saving cache file");
        Glib::file_set_contents (get_cache_file (), kf.to_data ());
    } catch (Glib::FileError& err) {
        g_warning ("Failed to save cache file: %s", err.what ().c_str ());
    }
}

void
Backend::register_plugin (const std::shared_ptr<AbstractPacketHandler>& handler) noexcept
{
    m_plugins[handler->get_packet_type ()] = handler;
}

std::shared_ptr<AbstractPacketHandler>
Backend::get_plugin (const std::string& capability) const noexcept
{
    if (m_plugins.find (capability) != m_plugins.end ()) return m_plugins.at (capability);
    return std::shared_ptr<AbstractPacketHandler> ();
}
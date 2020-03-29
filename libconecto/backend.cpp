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
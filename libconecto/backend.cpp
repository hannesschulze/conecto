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
#include <glibmm/miscutils.h>
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
    auto key_file  = Gio::File::create_for_path (Glib::build_filename (get_storage_dir (), "private.pem"));
    auto cert_file = Gio::File::create_for_path (Glib::build_filename (get_storage_dir (), "certificate.pem"));

    if (!key_file->query_exists () || !cert_file->query_exists ()) {
        std::string host_name = Glib::get_host_name ();
        std::string user = Glib::get_user_name ();
        // TODO
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

std::string
Backend::get_storage_dir () noexcept
{
    return Glib::build_filename (Glib::get_user_data_dir (), APP_NAME);
}
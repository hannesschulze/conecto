/* crypt.cpp
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

#include "crypt.h"
#include "exceptions.h"
#include <gnutls/x509.h>
#include <glibmm/datetime.h>
#include <giomm/file.h>
#include <arpa/inet.h>

using namespace Conecto;

std::shared_ptr<GnuTLSX509Certificate>
Crypt::generate_self_signed_cert (GnuTLSX509PrivateKey& key, const std::string& common_name)
{
    GnuTLSX509Certificate* cert_raw = nullptr;
    if (gnutls_x509_crt_init (&cert_raw) != 0)
        throw GnuTLSInitializationError ("Couldn't initialize certificate");
    std::shared_ptr<GnuTLSX509Certificate> cert (cert_raw, gnutls_x509_crt_deinit);
    auto start_time = Glib::DateTime::create_now_local ();
    auto end_time = start_time.add_years (10);

    gnutls_x509_crt_set_key (cert.get (), &key);
    gnutls_x509_crt_set_version (cert.get (), 1);
    gnutls_x509_crt_set_activation_time (cert.get (), (time_t) start_time.to_unix ());
    gnutls_x509_crt_set_expiration_time (cert.get (), (time_t) end_time.to_unix ());
    uint32_t serial = htonl (10);
    gnutls_x509_crt_set_serial (cert.get (), &serial, sizeof (uint32_t));

    std::list<DNSetting> dn_settings = {
        DNSetting (GNUTLS_OID_X520_ORGANIZATION_NAME, "conecto"),
        DNSetting (GNUTLS_OID_X520_ORGANIZATIONAL_UNIT_NAME, "conecto"),
        DNSetting (GNUTLS_OID_X520_COMMON_NAME, common_name)
    };

    for (const auto& dn_val : dn_settings) {
        int err = gnutls_x509_crt_set_dn_by_oid (cert.get (), dn_val.oid.c_str (), 0, dn_val.name.c_str (), dn_val.name.size ());
        if (err != GNUTLS_E_SUCCESS)
            g_warning ("Set dn failed for OID %s – %s, error: %d", dn_val.oid.c_str (), dn_val.name.c_str (), err);
    }

    int err = gnutls_x509_crt_sign (cert.get (), cert.get (), &key);
    g_assert (err == GNUTLS_E_SUCCESS);

    return cert;
}

std::shared_ptr<GnuTLSX509PrivateKey>
Crypt::generate_private_key ()
{
    GnuTLSX509PrivateKey* key_raw = nullptr;
    if (gnutls_x509_privkey_init (&key_raw) != 0)
        throw GnuTLSInitializationError ("Couldn't initialize private key");
    std::shared_ptr<GnuTLSX509PrivateKey> key (key_raw, gnutls_x509_privkey_deinit);

    gnutls_x509_privkey_generate (key.get (), GNUTLS_PK_RSA, 2048, 0);

    return key;
}

std::string
Crypt::export_certificate (GnuTLSX509Certificate& cert)
{
    std::array<uint8_t, 8192> buf;
    size_t size = 8192;

    int err = gnutls_x509_crt_export (&cert, GNUTLS_X509_FMT_PEM, buf.data (), &size);
    g_assert (err == GNUTLS_E_SUCCESS);
    std::string res ((char*) buf.data (), size);

    g_debug ("Actual certificate PEM size: %zu", size);
    g_debug ("Certificate PEM:\n%s", res.c_str ());

    return res;
}

std::string
Crypt::export_private_key (GnuTLSX509PrivateKey& key)
{
    std::array<uint8_t, 8192> buf;
    size_t size = 8192;

    int err = gnutls_x509_privkey_export_pkcs8 (&key, GNUTLS_X509_FMT_PEM, "", GNUTLS_PKCS_PLAIN, buf.data (), &size);
    g_assert (err == GNUTLS_E_SUCCESS);
    std::string res ((char*) buf.data (), size);

    g_debug ("Actual private key PEM size: %zu", size);
    g_debug ("Private key PEM:\n%s", res.c_str ());

    return res;
}

void
Crypt::export_to_file (const std::string& path, const std::string& data)
{
    try {
        auto file = Gio::File::create_for_path (path);
        std::string new_etag;
        file->replace_contents (data, "", new_etag, false, Gio::FILE_CREATE_PRIVATE | Gio::FILE_CREATE_REPLACE_DESTINATION);
    } catch (Gio::Error& err) {
        throw PEMWriteError (err.what ());
    }
}

void
Crypt::generate_key_cert (const std::string& key_path, const std::string& cert_path, const std::string& name)
{
    auto key = generate_private_key ();
    auto cert = generate_self_signed_cert (*key, name);

    export_to_file (cert_path, export_certificate (*cert));
    export_to_file (cert_path, export_private_key (*key));
}

std::shared_ptr<GnuTLSX509Certificate>
Crypt::cert_from_pem (const std::string& certificate_pem)
{
    gnutls_datum_t datum {
        .data = (unsigned char*) certificate_pem.c_str (),
        .size = certificate_pem.size ()
    };
    GnuTLSX509Certificate* cert_raw = nullptr;
    if (gnutls_x509_crt_init (&cert_raw) != 0)
        throw GnuTLSInitializationError ("Couldn't initialize certificate");
    std::shared_ptr<GnuTLSX509Certificate> cert (cert_raw, gnutls_x509_crt_deinit);

    int err = gnutls_x509_crt_import (cert.get (), &datum, GNUTLS_X509_FMT_PEM);
    g_assert (err == GNUTLS_E_SUCCESS);
    return cert;
}

std::string
Crypt::fingerprint_certificate (const std::string& certificate_pem)
{
    auto cert = cert_from_pem (certificate_pem);

    // TODO: Make digest configurable, for now assume it's SHA1
    std::array<uint8_t, 20> data;
    size_t size = 20;
    int err = gnutls_x509_crt_get_fingerprint (cert.get (), GNUTLS_DIG_SHA1, data.data (), &size);

    g_assert (err == GNUTLS_E_SUCCESS);
    g_assert (size == data.size ());

    return std::string ((char*) data.data (), size);
}
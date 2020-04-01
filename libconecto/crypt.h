/* crypt.h
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

#include <memory>
#include <string>
#include <vector>
#include <gnutls/gnutls.h>

namespace Conecto {

typedef struct gnutls_x509_crt_int     GnuTLSX509Certificate;
typedef struct gnutls_x509_privkey_int GnuTLSX509PrivateKey;

/**
 * Encryption helpers
 */
class Crypt {
  public:
    /**
     * @throw GnuTLSInitializationError
     */
    static std::shared_ptr<GnuTLSX509Certificate> generate_self_signed_cert (GnuTLSX509PrivateKey& key,
                                                                             const std::string&    common_name);
    /**
     * @throw GnuTLSInitializationError, PEMWriteError
     */
    static void generate_key_cert (const std::string& key_path, const std::string& cert_path, const std::string& name);
    /**
     * Produce a SHA1 fingerprint of the certificate
     *
     * @param certificate_pem PEM encoded certificate
     * @return SHA1 fingerprint as bytes
     * @throw GnuTLSInitializationError
     */
    static std::vector<uint8_t> fingerprint_certificate (const std::string& certificate_pem);

  private:
    struct DNSetting {
        DNSetting (const std::string& oid, const std::string& name)
            : oid (oid)
            , name (name)
        {
        }
        std::string oid, name;
    };

    /**
     * @throw GnuTLSInitializationError
     */
    static std::shared_ptr<GnuTLSX509PrivateKey> generate_private_key ();
    static std::string                           export_certificate (GnuTLSX509Certificate& cert);
    static std::string                           export_private_key (GnuTLSX509PrivateKey& key);
    /**
     * @throw PEMWriteError
     */
    static void export_to_file (const std::string& path, const std::string& data);
    /**
     * @throw GnuTLSInitializationError
     */
    static std::shared_ptr<GnuTLSX509Certificate> cert_from_pem (const std::string& certificate_pem);
};

} // namespace Conecto
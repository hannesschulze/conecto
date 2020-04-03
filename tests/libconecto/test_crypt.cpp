/* test_crypt.cpp
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

#include <gtest/gtest.h>
#include <conecto.h>
#include <glib/gstdio.h>
#include <giomm/file.h>

using namespace Conecto;

namespace {

constexpr char KEY_PATH[] = "/tmp/test-key.pem";
constexpr char CERT_PATH[] = "/tmp/test-cert.pem";

void
cleanup ()
{
    // Remove any older test files
    g_remove (KEY_PATH);
    g_remove (CERT_PATH);
    ASSERT_FALSE (Gio::File::create_for_path (KEY_PATH)->query_exists ());
    ASSERT_FALSE (Gio::File::create_for_path (CERT_PATH)->query_exists ());
}

} // namespace

TEST (GenerateTest, generate_test)
{
    cleanup ();

    Crypt::generate_key_cert (KEY_PATH, CERT_PATH, "test");
    ASSERT_TRUE (Gio::File::create_for_path (KEY_PATH)->query_exists ());
    ASSERT_TRUE (Gio::File::create_for_path (CERT_PATH)->query_exists ());
}

TEST (LoadTest, load_test)
{
    cleanup ();
    Crypt::generate_key_cert (KEY_PATH, CERT_PATH, "test");

    GError*          err = nullptr;
    GTlsCertificate* cert = g_tls_certificate_new_from_files (CERT_PATH, KEY_PATH, &err);
    ASSERT_EQ (err, nullptr);
    ASSERT_NE (cert, nullptr);
    g_object_unref (cert);
}

TEST (FingerprintTest, fingerprint_test)
{
    std::string pem = "-----BEGIN CERTIFICATE-----\n"
                      "MIIC8jCCAdoCAQowDQYJKoZIhvcNAQEFBQAwPzERMA8GA1UEChMIbWNvbm5lY3Qx\n"
                      "ETAPBgNVBAsTCG1jb25uZWN0MRcwFQYDVQQDDA5tYWNpZWtAY29yc2FpcjAeFw0x\n"
                      "NzA5MjQxOTU3NDVaFw0yNzA5MjQxOTU3NDVaMD8xETAPBgNVBAoTCG1jb25uZWN0\n"
                      "MREwDwYDVQQLEwhtY29ubmVjdDEXMBUGA1UEAwwObWFjaWVrQGNvcnNhaXIwggEi\n"
                      "MA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDBWMM09dCCGXjY1aZ261nCa8+q\n"
                      "3ejDOHf21+Mt6yJnoWjPGvTK21MbRFPkeQe62FJHF3q8iXe3sSEdFk/f56G8ZZ4t\n"
                      "Qw/ST//kxtf/CKHPuoZeAFgQpYEKE3GVUX5M3b8+4YSKSRXs3FE4D5awQVdstI31\n"
                      "N53plyOLdJe5IeK1kESsT0UgVo2RTHCOByel9WB2zcalQVTl8UxGKJcgrSuXj2f2\n"
                      "1SBxHupOX0Ej7vTr+gIBNBRbwbVdsafEW/gRngyCWIO30cPKoaUSkoCppXQ+6hYd\n"
                      "/aOt+6/bBOISGJdy6uyM74jqoEbEMdhUXHfPFNCelIABxUMez0SNrRmdag2VAgMB\n"
                      "AAEwDQYJKoZIhvcNAQEFBQADggEBAEnJ+IsjGvBmlWAg5vlmWUY0OVMJa8Gl9ept\n"
                      "HWLIjK16XARAOwIcePNfDPyITWaxT5YV+MZotm1m6HkY5rPCeOjV7nzHrHjTjZqO\n"
                      "sCmsSGcb9ZkEQfRNGTmFFthkcnfTU9mKh8oGc/a9r0DDgYcPSCgqERt2fgiBrt85\n"
                      "85PVl16fCSObVwOu5u5TtrfWkHpEHbjBU9AX52+IOYg7RsM7I4OnH+5svhmWqAxW\n"
                      "/PXFBB3q2nX2XXqFRhqeN9eOlHBQ5EoZh8GUp7vJyxp5eAS9g2KVtCBwTDElQt4D\n"
                      "4hu+QuzzEmoWY9w1R+hblNu/37mWkzFFrLqYlkNU2vbKkuWMOTg=\n"
                      "-----END CERTIFICATE-----";
    auto fingerprint = Crypt::fingerprint_certificate (pem);
    ASSERT_EQ (fingerprint.size (), 20);

    std::stringstream stream;
    stream << "sha1:";
    for (const auto& b : fingerprint) stream << std::setfill ('0') << std::setw (2) << std::hex << unsigned (b);
    std::string fingerprint_str = stream.str ();

    ASSERT_EQ (fingerprint_str, "sha1:eb2611a447085322b206fa61d4bc5869b4a55657");
}
/* exceptions.h
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

#include <exception>
#include <giomm/error.h>

namespace Conecto {

/**
 * Base class for all exceptions thrown in libconecto
 */
class Exception : public std::exception {
};

class BindSocketException : public Exception {
  public:
    BindSocketException (Gio::Error::Code code, const std::string& message)
        : m_code (code)
        , m_message (message) {}
    ~BindSocketException () {}
    const char* what () const throw () override { return m_message.c_str (); }
    Gio::Error::Code get_code () { return m_code; }

  private:
    Gio::Error::Code m_code;
    std::string m_message;
};

class MalformedPacketException : public Exception {
  public:
    MalformedPacketException (const std::string& message)
        : m_message (message) {}
    ~MalformedPacketException () {}
    const char* what () const throw () override { return m_message.c_str (); }

  private:
    std::string m_message;
};

class MalformedIdentityException : public Exception {
  public:
    MalformedIdentityException (const std::string& message)
        : m_message (message) {}
    ~MalformedIdentityException () {}
    const char* what () const throw () override { return m_message.c_str (); }

  private:
    std::string m_message;
};

class GnuTLSInitializationError : public Exception {
  public:
    GnuTLSInitializationError (const std::string& message)
        : m_message (message) {}
    ~GnuTLSInitializationError () {}
    const char* what () const throw () override { return m_message.c_str (); }

  private:
    std::string m_message;
};

class PEMWriteError : public Exception {
  public:
    PEMWriteError (const std::string& message)
        : m_message (message) {}
    ~PEMWriteError () {}
    const char* what () const throw () override { return m_message.c_str (); }

  private:
    std::string m_message;
};

class InvalidCertificateException : public Exception {
  public:
    InvalidCertificateException (const std::string& message)
        : m_message (message) {}
    ~InvalidCertificateException () {}
    const char* what () const throw () override { return m_message.c_str (); }

  private:
    std::string m_message;
};

class InvalidIpAddressException : public Exception {
  public:
    InvalidIpAddressException (const std::string& ip)
        : m_message ("Invalid ip address: " + ip) {}
    ~InvalidIpAddressException () {}
    const char* what () const throw () override { return m_message.c_str (); }

  private:
    std::string m_message;
};

}
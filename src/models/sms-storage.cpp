/* sms-storage.cpp
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

#include "sms-storage.h"
#include <conecto.h>

using namespace App::Models;

namespace {

struct QueryCbWrapper {
    QueryCbWrapper (std::function<void(const std::vector<std::string>&, const std::vector<std::string>&)>&& cb)
        : cb (std::move (cb)) {}
    ~QueryCbWrapper () {}
    std::function<void(const std::vector<std::string>&, const std::vector<std::string>&)> cb;
};

int static_query_cb (void* data, int n_columns, char** values, char** column_names) {
    QueryCbWrapper* wrapper = static_cast<QueryCbWrapper*> (data);

    std::vector<std::string> wrapper_values;
    wrapper_values.reserve (n_columns);
    std::vector<std::string> wrapper_column_names;
    wrapper_column_names.reserve (n_columns);

    for (int i = 0; i < n_columns; i++) {
        wrapper_values.emplace_back (values[i]);
        wrapper_column_names.emplace_back (column_names[i]);
    }
    wrapper->cb (wrapper_values, wrapper_column_names);
    return 0;
}

enum MessageColumns : size_t {
    ID = 0,
    PHONE = 1,
    MESSAGE = 2,
    DEVICE_ID = 3,
    DATETIME = 4,
    SENDER = 5
};

} // namespace

SMSStorage::SMSStorage ()
{
    {
        std::string path = Glib::build_filename (Conecto::Backend::get_instance ().get_config_dir (), "sms-storage.db");
        sqlite3* conn_raw = nullptr;
        int err = sqlite3_open (path.c_str (), &conn_raw);
        m_conn = std::shared_ptr<sqlite3> (conn_raw, sqlite3_close);
        if (err != SQLITE_OK)
            throw "Can't open SMS history database";
    }
    // Initial setup
    const static std::string query =
            "CREATE TABLE IF NOT EXISTS message("
            "    id             INTEGER      PRIMARY KEY AUTOINCREMENT,"
            "    phone_number   TEXT         NOT NULL,"
            "    message        TEXT         NOT NULL,"
            "    device_id      TEXT         NOT NULL,"
            "    date_received  DATETIME     CURRENT_TIMESTAMP,"
            "    sender         INTEGER      DEFAULT 1"
            ");";
    if (!exec_query (query))
        throw "An error occured while trying to set up the database structure";
}

SMSStorage::~SMSStorage ()
{
}

bool
SMSStorage::exec_query (const std::string& query, std::function<void(const std::vector<std::string>&, const std::vector<std::string>&)>&& cb)
{
    auto data = std::make_unique<QueryCbWrapper> (std::move (cb));
    int err = sqlite3_exec (m_conn.get (), query.c_str (), static_query_cb, data.get (), nullptr);
    if (err != SQLITE_OK) {
        g_warning ("Error while executing sqlite query: %s", query.c_str ());
        return false;
    }
    return true;
}

void
SMSStorage::add_sms (const Conecto::Device& device, const SMS& sms)
{
    g_debug ("Adding SMS in DB");
    std::stringstream query_stream;
    query_stream << "INSERT INTO message (phone_number, message, device_id, date_received, sender) "
        << "VALUES ('" << Glib::strescape (sms.phone_number) << "', '" << Glib::strescape (sms.message)
        << "', '" << Glib::strescape (device.get_device_id ()) << "', '" << sms.date_time.format ("%FT%H:%M:%S%z")
        << "', '" << sms.from << "');";
    exec_query (query_stream.str ());
}

std::list<SMSStorage::SMS>
SMSStorage::get_latest_sms_messages (const Conecto::Device& device, const std::string& phone_number)
{
    std::list<SMS> res;
    std::stringstream query_stream;
    query_stream << "SELECT * FROM (SELECT * FROM message WHERE phone_number = '" << Glib::strescape (phone_number)
        << "' AND device_id = '" + Glib::strescape (device.get_device_id ()) << "' ORDER BY date_received DESC LIMIT 20) "
        << "ORDER BY date_received ASC;";
    exec_query (query_stream.str (), [&res] (const std::vector<std::string>& vals, const std::vector<std::string>& cols) {
        res.emplace_back (vals.at (MessageColumns::MESSAGE), vals.at (MessageColumns::PHONE),
                          static_cast<SMS::FromType> (atoi (vals.at (MessageColumns::SENDER).c_str ())),
                          Glib::DateTime::create_from_iso8601 (vals.at (MessageColumns::DATETIME)));
    });
    return res;
}

std::list<SMSStorage::SMS>
SMSStorage::get_sms_messages_before (const Conecto::Device& device, const std::string& phone_number,
                                     const Glib::DateTime& datetime)
{
    std::list<SMS> res;
    std::stringstream query_stream;
    query_stream << "SELECT * FROM message WHERE phone_number = '" << Glib::strescape (phone_number)
        << "' AND device_id = '" + Glib::strescape (device.get_device_id ()) << "' AND date_received < '"
        << datetime.format ("%FT%H:%M:%S%z") << "' ORDER BY date_received DESC LIMIT 10;";
    exec_query (query_stream.str (), [&res] (const std::vector<std::string>& vals, const std::vector<std::string>& cols) {
        res.emplace_back (vals.at (MessageColumns::MESSAGE), vals.at (MessageColumns::PHONE),
                          static_cast<SMS::FromType> (atoi (vals.at (MessageColumns::SENDER).c_str ())),
                          Glib::DateTime::create_from_iso8601 (vals.at (MessageColumns::DATETIME)));
    });
    return res;
}

std::list<std::string>
SMSStorage::get_phone_numbers (const Conecto::Device& device)
{
    std::list<std::string> res;
    std::stringstream query_stream;
    query_stream << "SELECT DISTINCT phone_number FROM message WHERE device_id = '"
        << Glib::strescape (device.get_device_id ()) << "';";
    exec_query (query_stream.str (), [&res] (const std::vector<std::string>& vals, const std::vector<std::string>& cols) {
        res.emplace_back (vals.at (0));
    });
    return res;
}
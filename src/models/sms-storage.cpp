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
    std::unique_ptr<QueryCbWrapper> wrapper (static_cast<QueryCbWrapper*> (data));

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
            "CREATE TABLE message("
            "    phone_number   TEXT         PRIMARY KEY NOT NULL,"
            "    message        TEXT         NOT NULL,"
            "    device_id      TEXT         NOT NULL,"
            "    date_received  DATETIME     CURRENT_TIMESTAMP"
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
    int err = sqlite3_exec (m_conn.get (), query.c_str (), static_query_cb, new QueryCbWrapper (std::move (cb)), nullptr);
    if (err != SQLITE_OK) {
        g_warning ("Error while executing sqlite query: %s", query.c_str ());
        return false;
    }
    return true;
}
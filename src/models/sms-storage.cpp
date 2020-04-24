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

    // Fetch contacts
    fetch_available_contacts ();
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

void
SMSStorage::fetch_available_contacts ()
{
    m_available_contacts.clear ();
    m_phone_numbers_map.clear ();
    m_aggregator.reset (folks_individual_aggregator_dup (), g_object_unref);
    GeeMap* individuals = folks_individual_aggregator_get_individuals (m_aggregator.get ());

    std::shared_ptr<GeeCollection> values (gee_map_get_values (individuals), g_object_unref);
    m_available_contacts.reserve (gee_collection_get_size (values.get ()));
    std::shared_ptr<GeeIterator> it (gee_iterable_iterator (GEE_ITERABLE (values.get ())), g_object_unref);
    int i = 0;
    while (gee_iterator_next (it.get ())) {
        std::shared_ptr<FolksIndividual> individual (
            static_cast<FolksIndividual*> (gee_iterator_get (it.get ())), g_object_unref);
        std::vector<std::string> phone_numbers;
        GeeCollection* gee_phone_numbers = GEE_COLLECTION (
            folks_phone_details_get_phone_numbers ((FolksPhoneDetails*) individual.get ()));
        if (gee_collection_get_size (gee_phone_numbers) < 1) continue;
        phone_numbers.reserve (gee_collection_get_size (gee_phone_numbers));
        std::shared_ptr<GeeIterator> phone_number_it (gee_iterable_iterator (GEE_ITERABLE (gee_phone_numbers)),
                                                      g_object_unref);
        while (gee_iterator_next (phone_number_it.get ())) {
            std::shared_ptr<FolksAbstractFieldDetails> phone_number_field (
                static_cast<FolksAbstractFieldDetails*> (gee_iterator_get (phone_number_it.get ())), g_object_unref);
            const gchar* phone_number = static_cast<const gchar*> (
                folks_abstract_field_details_get_value (phone_number_field.get ()));
            phone_numbers.emplace_back (phone_number);
            m_phone_numbers_map.insert ({ phone_number, i });
        }
        i++;
        m_available_contacts.emplace_back (folks_individual_get_display_name (individual.get ()),
                                           std::move (phone_numbers));
    }

    // Connect to changes
    g_signal_connect (m_aggregator.get (), "individuals-changed-detailed", G_CALLBACK (on_individuals_changed), this);

    folks_individual_aggregator_prepare (m_aggregator.get (), (GAsyncReadyCallback) on_prepare_cb, this);
}

void
SMSStorage::on_prepare_cb (GObject* source_object, GAsyncResult* res, SMSStorage* self)
{
    GError* err = nullptr;
    folks_individual_aggregator_prepare_finish (self->m_aggregator.get (), res, &err);
    if (err) {
        g_critical ("Could not prepare Folks.IndividualAggregator (%s)", err->message);
        g_error_free (err);
    }
}

void
SMSStorage::on_individuals_changed (FolksIndividualAggregator* _sender, GeeMultiMap* changes, SMSStorage* self)
{
    (void) _sender;
    std::shared_ptr<GeeCollection> values (gee_multi_map_get (changes, nullptr), g_object_unref);
    std::shared_ptr<GeeIterator> it (gee_iterable_iterator (GEE_ITERABLE (values.get ())), g_object_unref);
    int i = self->m_available_contacts.size ();
    while (gee_iterator_next (it.get ())) {
        std::shared_ptr<FolksIndividual> individual (
            static_cast<FolksIndividual*> (gee_iterator_get (it.get ())), g_object_unref);
        std::vector<std::string> phone_numbers;
        GeeCollection* gee_phone_numbers = GEE_COLLECTION (
            folks_phone_details_get_phone_numbers ((FolksPhoneDetails*) individual.get ()));
        if (gee_collection_get_size (gee_phone_numbers) < 1) continue;
        phone_numbers.reserve (gee_collection_get_size (gee_phone_numbers));
        std::shared_ptr<GeeIterator> phone_number_it (gee_iterable_iterator (GEE_ITERABLE (gee_phone_numbers)),
                                                      g_object_unref);
        while (gee_iterator_next (phone_number_it.get ())) {
            std::shared_ptr<FolksAbstractFieldDetails> phone_number_field (
                static_cast<FolksAbstractFieldDetails*> (gee_iterator_get (phone_number_it.get ())), g_object_unref);
            const gchar* phone_number = static_cast<const gchar*> (
                folks_abstract_field_details_get_value (phone_number_field.get ()));
            phone_numbers.emplace_back (phone_number);
            self->m_phone_numbers_map.insert ({ phone_number, i });
        }
        i++;
        self->m_available_contacts.emplace_back (folks_individual_get_display_name (individual.get ()),
                                                 std::move (phone_numbers));
    }
    self->m_signal_available_contacts_changed.emit ();
}

std::vector<SMSStorage::Contact>
SMSStorage::get_conversation_contacts (const Conecto::Device& device)
{
    auto phone_numbers = get_phone_numbers (device);
    std::vector<Contact> res;
    res.reserve (phone_numbers.size ());
    for (const auto& phone_number : phone_numbers) {
        if (m_phone_numbers_map.find (phone_number) != m_phone_numbers_map.end ()) {
            res.push_back (m_available_contacts.at (m_phone_numbers_map.at (phone_number)));
            continue;
        }
        res.emplace_back (std::string (), std::vector<std::string> ({ phone_number }));
    }
    return res;
}

const std::vector<SMSStorage::Contact>&
SMSStorage::get_available_contacts ()
{
    return m_available_contacts;
}
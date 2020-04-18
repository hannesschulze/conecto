/* sms-storage.h
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

#include <gtkmm.h>
#include <sqlite3.h>

// forward declarations
namespace Conecto {
class Device;
}

namespace App {
namespace Models {

/**
 * @brief A storage for SMS messages built using sqlite
 */
class SMSStorage {
  public:
    /**
     * @brief Create a new SMSStorage-model
     */
    SMSStorage ();
    ~SMSStorage ();

    struct SMS {
        enum FromType : int { FROM_ME = 0, FROM_CONTACT = 1 };

        SMS (const std::string& message, const std::string& phone_number, FromType from, const Glib::DateTime& date_time)
            : message (message), phone_number (phone_number), from (from), date_time (date_time) {}

        std::string message;
        std::string phone_number;
        FromType from;
        Glib::DateTime date_time;
    };

    /**
     * @brief Add a new SMS message to the database
     */
    void add_sms (const Conecto::Device& device, const SMS& sms);

    /**
     * @brief Get the latest 20 SMS messages received/sent by @param device from/to @param phone_number
     */
    std::list<SMS> get_latest_sms_messages (const Conecto::Device& device, const std::string& phone_number);

    /**
     * @brief Get the 10 most recent SMS messages received/sent by @param device from/to @param phone_number before @param datetime
     */
    std::list<SMS> get_sms_messages_before (const Conecto::Device& device, const std::string& phone_number,
                                            const Glib::DateTime& datetime);

    /**
     * @brief Get a list of phone numbers for which conversations exist through @param device
     */
    std::list<std::string> get_phone_numbers (const Conecto::Device& device);

    SMSStorage (const SMSStorage&) = delete;
    SMSStorage& operator= (const SMSStorage&) = delete;

  private:
    std::shared_ptr<sqlite3> m_conn;

    bool exec_query (const std::string& query, std::function<void(const std::vector<std::string>& /* values */, const std::vector<std::string>& /* column_names */)>&& cb = [] (const std::vector<std::string>&, const std::vector<std::string>&) {});
};

} // namespace Models
} // namespace App
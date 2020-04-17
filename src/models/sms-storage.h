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

namespace App {
namespace Models {

/**
 * @brief A storage for SMS messages built using sqlite
 */
class SMSStorage {
  public:
    /**
     * Create a new SMSStorage-model
     */
    SMSStorage ();
    ~SMSStorage ();

    SMSStorage (const SMSStorage&) = delete;
    SMSStorage& operator= (const SMSStorage&) = delete;

  private:
    std::shared_ptr<sqlite3> m_conn;

    bool exec_query (const std::string& query, std::function<void(const std::vector<std::string>& /* values */, const std::vector<std::string>& /* column_names */)>&& cb = [] (const std::vector<std::string>&, const std::vector<std::string>&) {});
};

} // namespace Models
} // namespace App
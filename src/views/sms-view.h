/* sms-view.h
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
#include "../models/sms-storage.h"

namespace App {
namespace Views {

/**
 * @brief A view for listing received messages and sending SMS messages
 *
 * Connected to the following model: @p App::Models::SMSStorage
 */
class SMSView : public Gtk::Paned {
  public:
    /**
     * @brief Create an SMS view
     */
    static std::shared_ptr<SMSView> create (const std::shared_ptr<Models::SMSStorage>& model);
    ~SMSView () {}

    SMSView (const SMSView&) = delete;
    SMSView& operator= (const SMSView&) = delete;

    friend Gtk::Builder;

  private:
    SMSView (BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder> glade_ref);

    std::shared_ptr<Models::SMSStorage> m_model;
    Glib::RefPtr<Gtk::Builder>          m_builder;
};

} // namespace Views
} // namespace App
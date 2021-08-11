/* notification-row.h
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
#include "../models/notifications-list.h"

namespace App {
namespace Widgets {

/**
 * @brief A single listbox row representing a notification
 */
class NotificationRow : public Gtk::ListBoxRow {
  public:
    /**
     * @brief Construct a new ListBoxRow
     */
    static std::shared_ptr<NotificationRow> create (const Glib::RefPtr<Models::NotificationsList>& model);
    ~NotificationRow () {}

    /** @brief Update the view */
    void update (const Gtk::TreeIter& iter);

    NotificationRow (const NotificationRow&) = delete;
    NotificationRow& operator= (const NotificationRow&) = delete;

  private:
    NotificationRow (const Glib::RefPtr<Models::NotificationsList>& model);

    Glib::RefPtr<Models::NotificationsList> m_model;
    Glib::RefPtr<Gtk::Builder>              m_builder;

    // Widgets from the Gtk::Builder
    Gtk::Label*  m_lbl_title;
    Gtk::Label*  m_lbl_text;
    Gtk::Label*  m_lbl_time;
    Gtk::Label*  m_lbl_app_name;
    Gtk::Button* m_btn_dismiss;

    // Current id
    std::string m_id;

    void on_dismiss ();
};

} // namespace Widgets
} // namespace App
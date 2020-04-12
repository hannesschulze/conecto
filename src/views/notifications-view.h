/* notifications-view.h
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
namespace Views {

/**
 * @brief A list showing a list of most recent notifications
 * 
 * Connected to the following model: @p App::Models::NotificationsList
 */
class NotificationsView : public Gtk::TreeView {
  public:
    /**
     * @brief Create a list of notifications
     */
    NotificationsView ();
    ~NotificationsView () {}

    /** Update the model */
    void update (const Glib::RefPtr<Models::NotificationsList>& model);

    NotificationsView (const NotificationsView&) = delete;
    NotificationsView& operator= (const NotificationsView&) = delete;
};

} // namespace Views
} // namespace App
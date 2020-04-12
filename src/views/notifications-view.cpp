/* notifications-view.cpp
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

#include "notifications-view.h"

using namespace App::Views;
using namespace App::Models;

NotificationsView::NotificationsView ()
    : Gtk::TreeView ()
{
    set_vexpand (true);
}

void
NotificationsView::update (const Glib::RefPtr<NotificationsList>& model)
{
    remove_all_columns ();
    set_model (model);
    append_column ("Application", model->column_app_name);
    append_column ("Id", model->column_id);
    append_column ("Title", model->column_title);
    append_column ("Body", model->column_body);
    append_column ("Timestamp", model->column_time);
}
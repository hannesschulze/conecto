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
#include "../widgets/notification-row.h"

using namespace App::Views;
using namespace App::Models;

NotificationsView::NotificationsView ()
    : Gtk::ListBox ()
{
    set_vexpand (true);
}

void
NotificationsView::update (const Glib::RefPtr<NotificationsList>& model)
{
    for (auto& connection : m_model_connections) connection.disconnect ();
    m_model = model;
    m_row_refs.clear ();
    m_model_connections.push_back (
            model->signal_row_inserted ().connect (sigc::mem_fun (*this, &NotificationsView::on_row_inserted)));
    m_model_connections.push_back (
            model->signal_row_changed ().connect (sigc::mem_fun (*this, &NotificationsView::on_row_changed)));
    m_model_connections.push_back (
            model->signal_row_deleted ().connect (sigc::mem_fun (*this, &NotificationsView::on_row_deleted)));
}

void
NotificationsView::on_row_inserted (const Gtk::TreeModel::Path& path, const Gtk::TreeModel::iterator& it)
{
    auto row = Widgets::NotificationRow::create (m_model);
    insert (*row, path[0]);
    row->show_all ();
    row->update (it);
    m_row_refs.push_back (row);
}

void
NotificationsView::on_row_deleted (const Gtk::TreeModel::Path& path)
{
    if (path[0] >= static_cast<int> (get_children ().size ())) return;
    remove (*get_children ()[path[0]]);
}

void
NotificationsView::on_row_changed (const Gtk::TreeModel::Path& path, const Gtk::TreeModel::iterator& it)
{
    if (path[0] >= static_cast<int> (get_children ().size ())) return;
    dynamic_cast<Widgets::NotificationRow*> (get_children ()[path[0]])->update (it);
}
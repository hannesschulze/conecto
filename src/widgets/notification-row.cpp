/* notification-row.cpp
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

#include "notification-row.h"

using namespace App::Widgets;

NotificationRow::NotificationRow (const Glib::RefPtr<Models::NotificationsList>& model)
    : Gtk::ListBoxRow ()
    , m_model (model)
    , m_builder (
              Gtk::Builder::create_from_resource ("/com/github/hannesschulze/conecto/ui/widgets/notification-row.ui"))
    , m_lbl_title (nullptr)
    , m_lbl_text (nullptr)
    , m_lbl_time (nullptr)
    , m_lbl_app_name (nullptr)
    , m_btn_dismiss (nullptr)
{
    Gtk::Grid* widget = nullptr;
    m_builder->get_widget ("ConectoWidgetsNotificationRow", widget);
    m_builder->get_widget ("lbl_title", m_lbl_title);
    m_builder->get_widget ("lbl_text", m_lbl_text);
    m_builder->get_widget ("lbl_time", m_lbl_time);
    m_builder->get_widget ("lbl_app_name", m_lbl_app_name);
    m_builder->get_widget ("btn_dismiss", m_btn_dismiss);
    add (*widget);

    set_hexpand (true);
    get_style_context ()->add_class (GTK_STYLE_CLASS_MENUITEM);
    show_all ();

    m_btn_dismiss->signal_clicked ().connect (sigc::mem_fun (*this, &NotificationRow::on_dismiss));
}

std::shared_ptr<NotificationRow>
NotificationRow::create (const Glib::RefPtr<Models::NotificationsList>& model)
{
    return std::shared_ptr<NotificationRow> (new NotificationRow (model));
}

void
NotificationRow::update (const Gtk::TreeIter& iter)
{
    m_id = iter->get_value (m_model->column_id);
    m_lbl_title->set_label ("<b>" + iter->get_value (m_model->column_title) + "</b>");
    m_lbl_time->set_label (iter->get_value (m_model->column_time));
    Glib::ustring message = iter->get_value (m_model->column_body);
    m_lbl_text->set_visible (message != Glib::ustring ());
    m_lbl_text->set_label (message);
    m_lbl_app_name->set_label (iter->get_value (m_model->column_app_name));
}

void
NotificationRow::on_dismiss ()
{
    if (m_id != std::string ()) m_model->dismiss (m_id);
}
/* empty-selection-view.cpp
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

#include "empty-selection-view.h"

using namespace App::Views::Main;

EmptySelectionView::EmptySelectionView (BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder> glade_ref)
    : Gtk::Grid (cobject)
    , m_builder (glade_ref)
{
}

std::shared_ptr<EmptySelectionView>
EmptySelectionView::create ()
{
    EmptySelectionView* res = nullptr;
    auto                builder = Gtk::Builder::create_from_resource (
            "/com/github/hannesschulze/conecto/ui/views/main/empty-selection-view.ui");
    builder->get_widget_derived ("ConectoViewsMainEmptySelectionView", res);
    return std::shared_ptr<EmptySelectionView> (res);
}
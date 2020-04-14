/* empty-selection-view.h
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

namespace App {
namespace Views {
namespace Main {

/**
 * @brief A view shown if no device is selected
 *
 * This is just a simple screen set up using Glade
 */
class EmptySelectionView : public Gtk::Grid {
  public:
    /**
     * @brief Construct the view
     */
    static std::shared_ptr<EmptySelectionView> create ();
    ~EmptySelectionView () {}

    EmptySelectionView (const EmptySelectionView&) = delete;
    EmptySelectionView& operator= (const EmptySelectionView&) = delete;

    friend Gtk::Builder;

  private:
    EmptySelectionView (BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder> glade_ref);

    Glib::RefPtr<Gtk::Builder> m_builder;
};

} // namespace Main
} // namespace Views
} // namespace App
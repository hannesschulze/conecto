/* warning-view.h
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
namespace Dock {

/**
 * @brief A view shown if the connected device is unavailable or has been removed
 *
 * It offers the user the option to remove the dock item
 */
class WarningView : public Gtk::Grid {
  public:
    /**
     * @brief Construct the view
     */
    static std::shared_ptr<WarningView> create (const Glib::ustring& id);
    ~WarningView () {}

    WarningView (const WarningView&) = delete;
    WarningView& operator= (const WarningView&) = delete;

    friend Gtk::Builder;

    using type_signal_close_popover = sigc::signal<void>;
    /**
     * Emitted when the popover should be closed (e.g. when the device has been removed)
     */
    type_signal_close_popover signal_close_popover () { return m_signal_close_popover; }

  private:
    WarningView (BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder> glade_ref);

    Glib::ustring              m_id;
    Glib::RefPtr<Gtk::Builder> m_builder;

    // Widgets from the Gtk::Builder
    Gtk::Button* m_btn_remove;

    void on_btn_remove_clicked ();

    type_signal_close_popover m_signal_close_popover;
};

} // namespace Dock
} // namespace Views
} // namespace App
/* popover-window.h
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
#include <granite.h>

namespace App {
namespace Widgets {

/**
 * @brief A popover-like application window
 *
 * Modified version of Messenger's ApplicationPopOver class:
 * https://github.com/aprilis/messenger/blob/master/src/ApplicationPopOver.vala
 */
class PopoverWindow : public Gtk::ApplicationWindow {
  public:
    /**
     * @brief Create a popover window
     *
     * The window should be attached to a @p Gtk::Application using @p Gtk::Application::add_window
     */
    static std::shared_ptr<PopoverWindow> create ();
    ~PopoverWindow () {}

    /**
     * Set the position this popover is pointing to on the screen and the arrow position
     *
     * @param x The horizontal position
     * @param y The vertical position
     * @param position The arrow's position
     */
    void set_pointing_to (int x, int y, Gtk::PositionType position);
    /**
     * Close the popover
     */
    void close_popover ();

    PopoverWindow (const PopoverWindow&) = delete;
    PopoverWindow& operator= (const PopoverWindow&) = delete;

  protected:
    PopoverWindow ();

    void set_path (cairo_t* cr, double x, double y, double width, double height, double border_radius);
    void compute_shadow (int width, int height);
    void update_child_margin ();

    bool on_draw (const Cairo::RefPtr<Cairo::Context>& cr) override;
    void on_size_allocate (Gtk::Allocation& allocation) override;
    void on_add (Gtk::Widget* widget) override;
    bool on_button_release_event (GdkEventButton* release_event) override;
    bool on_key_release_event (GdkEventKey* key_event) override;
    bool on_window_state_event (GdkEventWindowState* window_state_event) override;
    void on_show () override;

  private:
    std::shared_ptr<GraniteDrawingBufferSurface> m_main_buffer;

    int               m_arrow_offset;
    Gtk::PositionType m_arrow_position;
    int               m_old_width, m_old_height;
    bool m_do_not_close; // prevents closing for the first 200ms because events are emitted during this time
};

} // namespace Widgets
} // namespace App
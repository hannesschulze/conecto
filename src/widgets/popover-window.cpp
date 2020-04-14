/* popover-window.cpp
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

#include "popover-window.h"
#include "../utils/focus.h"
#include <gdk/gdkkeysyms.h>

using namespace App::Widgets;

namespace {

constexpr int ARROW_WIDTH = 20;
constexpr int ARROW_HEIGHT = 10;
constexpr int BORDER_RADIUS = 6;
constexpr int BORDER_WIDTH = 1;
constexpr int SHADOW_SIZE = 20;

} // namespace

std::shared_ptr<PopoverWindow>
PopoverWindow::create ()
{
    return std::shared_ptr<PopoverWindow> (new PopoverWindow);
}

PopoverWindow::PopoverWindow ()
    : Gtk::ApplicationWindow ()
    , m_arrow_offset (0)
    , m_old_width (0)
    , m_old_height (0)
    , m_do_not_close (false)
{
    set_app_paintable (true);
    set_decorated (false);
    set_deletable (false);
    gtk_widget_set_visual (GTK_WIDGET (gobj ()), get_screen ()->get_rgba_visual ()->gobj ());

    set_type_hint (Gdk::WINDOW_TYPE_HINT_DOCK);
    set_skip_taskbar_hint (true);
    set_skip_pager_hint (true);
    set_keep_above (true);
    stick ();
}

bool
PopoverWindow::on_draw (const Cairo::RefPtr<Cairo::Context>& cr)
{
    auto style_context = get_style_context ();

    cr->begin_new_path ();
    cairo_append_path (cr->cobj (),
                       cairo_copy_path (granite_drawing_buffer_surface_get_context (m_main_buffer.get ())));
    cr->clip ();
    style_context->render_background (cr, 0, 0, granite_drawing_buffer_surface_get_width (m_main_buffer.get ()),
                                      granite_drawing_buffer_surface_get_height (m_main_buffer.get ()));
    bool ret = ApplicationWindow::on_draw (cr);
    cr->reset_clip ();
    cairo_set_source_surface (cr->cobj (), granite_drawing_buffer_surface_get_surface (m_main_buffer.get ()), 0, 0);
    cr->paint ();

    return ret;
}

void
PopoverWindow::on_size_allocate (Gtk::Allocation& allocation)
{
    Gtk::ApplicationWindow::on_size_allocate (allocation);
    int w = get_allocated_width ();
    int h = get_allocated_height ();
    if (m_old_width == w && m_old_height == h) return;

    compute_shadow (w, h);

    m_old_width = w;
    m_old_height = h;
}

void
PopoverWindow::set_path (cairo_t* cr, double x, double y, double width, double height, double border_radius)
{
    double arrow_offset;
    int    arrow_side_offset = ARROW_HEIGHT - SHADOW_SIZE;
    if (m_arrow_position == Gtk::POS_TOP || m_arrow_position == Gtk::POS_BOTTOM) {
        arrow_offset = (width - ARROW_WIDTH) / 2 + m_arrow_offset + x;
        height -= arrow_side_offset;
        if (m_arrow_position == Gtk::POS_TOP) y += arrow_side_offset;
    } else {
        arrow_offset = (height - ARROW_WIDTH) / 2 + m_arrow_offset + y;
        width -= arrow_side_offset;
        if (m_arrow_position == Gtk::POS_LEFT) x += arrow_side_offset;
    }

    cairo_arc (cr, x + border_radius, y + border_radius, border_radius, M_PI, M_PI * 1.5);
    if (m_arrow_position == Gtk::POS_TOP) {
        cairo_line_to (cr, arrow_offset, y);
        cairo_rel_line_to (cr, ARROW_WIDTH / 2.0, -ARROW_HEIGHT);
        cairo_rel_line_to (cr, ARROW_WIDTH / 2.0, ARROW_HEIGHT);
    }
    cairo_arc (cr, x + width - border_radius, y + border_radius, border_radius, M_PI * 1.5, M_PI * 2.0);
    if (m_arrow_position == Gtk::POS_RIGHT) {
        cairo_line_to (cr, x + width, arrow_offset);
        cairo_rel_line_to (cr, ARROW_HEIGHT, ARROW_WIDTH / 2.0);
        cairo_rel_line_to (cr, -ARROW_HEIGHT, ARROW_WIDTH / 2.0);
    }
    cairo_arc (cr, x + width - border_radius, y + height - border_radius, border_radius, 0, M_PI * 0.5);
    if (m_arrow_position == Gtk::POS_BOTTOM) {
        cairo_line_to (cr, arrow_offset + ARROW_WIDTH, y + height);
        cairo_rel_line_to (cr, -ARROW_WIDTH / 2.0, ARROW_HEIGHT);
        cairo_rel_line_to (cr, -ARROW_WIDTH / 2.0, -ARROW_HEIGHT);
    }
    cairo_arc (cr, x + border_radius, y + height - border_radius, border_radius, M_PI * 0.5, M_PI);
    if (m_arrow_position == Gtk::POS_LEFT) {
        cairo_line_to (cr, x, arrow_offset + ARROW_WIDTH);
        cairo_rel_line_to (cr, -ARROW_HEIGHT, -ARROW_WIDTH / 2.0);
        cairo_rel_line_to (cr, ARROW_HEIGHT, -ARROW_WIDTH / 2.0);
    }
    cairo_close_path (cr);
}

void
PopoverWindow::compute_shadow (int width, int height)
{
    m_main_buffer.reset (granite_drawing_buffer_surface_new (width, height), g_object_unref);

    // Shadow first
    set_path (granite_drawing_buffer_surface_get_context (m_main_buffer.get ()), SHADOW_SIZE + BORDER_WIDTH / 2.0,
              SHADOW_SIZE + BORDER_WIDTH / 2.0, width - SHADOW_SIZE * 2 - BORDER_WIDTH,
              height - SHADOW_SIZE * 2 - BORDER_WIDTH, BORDER_RADIUS);
    cairo_t* cr = granite_drawing_buffer_surface_get_context (m_main_buffer.get ());
    cairo_set_source_rgba (cr, 0.0, 0.0, 0.0, 0.4);
    cairo_fill_preserve (cr);
    granite_drawing_buffer_surface_exponential_blur (m_main_buffer.get (), SHADOW_SIZE / 2 - 1);
    // Reset the context, just in case
    cr = granite_drawing_buffer_surface_get_context (m_main_buffer.get ());

    // Background
    cairo_set_source_rgba (cr, 1, 1, 1, 1);
    cairo_set_operator (cr, CAIRO_OPERATOR_CLEAR);
    cairo_fill_preserve (cr);

    // Outer border
    cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);
    cairo_set_line_width (cr, BORDER_WIDTH);
    cairo_set_source_rgba (cr, 0.5, 0.5, 0.5, 0.5);
    cairo_stroke_preserve (cr);
}

void
PopoverWindow::update_child_margin ()
{
    Gtk::Widget* child = get_child ();
    if (!child) return;

    child->set_margin_start (SHADOW_SIZE);
    child->set_margin_end (SHADOW_SIZE);
    child->set_margin_top (SHADOW_SIZE);
    child->set_margin_bottom (SHADOW_SIZE);
    switch (m_arrow_position) {
    case Gtk::POS_LEFT:
        child->set_margin_start (ARROW_HEIGHT);
        break;
    case Gtk::POS_RIGHT:
        child->set_margin_end (ARROW_HEIGHT);
        break;
    case Gtk::POS_TOP:
        child->set_margin_top (ARROW_HEIGHT);
        break;
    case Gtk::POS_BOTTOM:
        child->set_margin_bottom (ARROW_HEIGHT);
        break;
    default:
        break;
    }
}

void
PopoverWindow::on_add (Gtk::Widget* widget)
{
    Gtk::ApplicationWindow::on_add (widget);
    update_child_margin ();
}

void
PopoverWindow::set_pointing_to (int x, int y, Gtk::PositionType position)
{
    if (m_arrow_position != position) {
        m_arrow_position = position;
        update_child_margin ();
    }
    int width, height;
    get_size_request (width, height);
    x -= width / 2;
    y -= height;
    y += SHADOW_SIZE;
    int            cx = x + width / 2;
    int            cy = y + height / 2;
    Gdk::Rectangle rect;
    get_screen ()->get_monitor_geometry (get_screen ()->get_monitor_at_point (cx, cy), rect);
    int offset_x = std::max (0, std::min (rect.get_x () - x, rect.get_x () + rect.get_width () - x - width));
    int offset_y = std::max (0, std::min (rect.get_y () - y, rect.get_y () + rect.get_height () - y - height));
    if (position == Gtk::POS_BOTTOM || position == Gtk::POS_TOP)
        m_arrow_offset = -offset_x;
    else
        m_arrow_offset = -offset_y;
    move (x + offset_x, y + offset_y);
}

bool
PopoverWindow::on_button_release_event (GdkEventButton* event)
{
    Gtk::ApplicationWindow::on_button_release_event (event);
    if (event->x < 0 || event->y < 0 || event->x >= get_allocated_width () || event->y >= get_allocated_height ())
        close_popover ();
    return false;
}

bool
PopoverWindow::on_key_release_event (GdkEventKey* event)
{
    Gtk::ApplicationWindow::on_key_release_event (event);
    if (event->keyval == GDK_KEY_Escape) close_popover ();
    return false;
}

bool
PopoverWindow::on_window_state_event (GdkEventWindowState* event)
{
    Gtk::ApplicationWindow::on_window_state_event (event);
    if (!m_do_not_close) close_popover ();
    return false;
}

void
PopoverWindow::on_show ()
{
    m_do_not_close = true;
    Gtk::ApplicationWindow::on_show ();
    Utils::Focus::grab (get_window (), false, true);
    Glib::signal_timeout ().connect_once ([this] () { m_do_not_close = false; }, 200);
}

void
PopoverWindow::close_popover ()
{
    Utils::Focus::ungrab ();
    hide ();
}
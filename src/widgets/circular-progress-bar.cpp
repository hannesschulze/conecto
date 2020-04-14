/* circular-progress-bar.cpp
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

#include "circular-progress-bar.h"
#include <cassert>

using namespace App::Widgets;

namespace {

constexpr int MIN_DIAMETER = 80;

template<class T>
constexpr const T&
clamp (const T& v, const T& lo, const T& hi)
{
    assert (!(hi < lo));
    return (v < lo) ? lo : (hi < v) ? hi : v;
}

} // namespace

CircularProgressBar::CircularProgressBar ()
    : Glib::ObjectBase ("circular-progress-bar")
    , Gtk::Bin ()
    , m_progress (0.0)
{
    set_size_request (120, 120);
}

Gtk::SizeRequestMode
CircularProgressBar::get_request_mode_vfunc () const
{
    return Gtk::SIZE_REQUEST_CONSTANT_SIZE;
}

void
CircularProgressBar::get_preferred_width_vfunc (int& min_width, int& natural_width) const
{
    int diameter = calculate_radius () * 2;
    min_width = MIN_DIAMETER;
    natural_width = std::max (diameter, MIN_DIAMETER);
}

void
CircularProgressBar::get_preferred_height_vfunc (int& min_height, int& natural_height) const
{
    int diameter = calculate_radius () * 2;
    min_height = MIN_DIAMETER;
    natural_height = std::max (diameter, MIN_DIAMETER);
}

bool
CircularProgressBar::on_draw (const Cairo::RefPtr<Cairo::Context>& cr)
{
    Gtk::Bin::on_draw (cr);

    cr->save ();

    const Gtk::Allocation allocation = get_allocation ();

    int center_x = (allocation.get_width () - 2) / 2;
    int center_y = (allocation.get_height () - 2) / 2;
    int radius = calculate_radius () - 1;

    Gtk::StateFlags state = get_state_flags ();
    bool            in_focus = !((state & Gtk::STATE_FLAG_BACKDROP) == Gtk::STATE_FLAG_BACKDROP);

    bool        found = false;
    std::string theme = Glib::getenv ("GTK_THEME");
    bool        dark = Gtk::Settings::get_default ()->property_gtk_application_prefer_dark_theme ().get_value () ||
                (found && (g_str_has_suffix (theme.c_str (), ":dark") || g_str_has_suffix (theme.c_str (), "-dark")));

    // TODO: Do rendering using an offscreen progress bar
    if (dark) {
        if (in_focus) {
            draw_stroke (cr, radius, 5, 0, center_x, center_y, 1.0, Gdk::RGBA ("#262626"));
            draw_stroke (cr, radius, 5, 0, center_x, center_y, get_progress (), Gdk::RGBA ("#1c71d8"));
        } else {
            draw_stroke (cr, radius, 5, 0, center_x, center_y, 1.0, Gdk::RGBA ("#121212"));
            draw_stroke (cr, radius, 5, 0, center_x, center_y, get_progress (), Gdk::RGBA ("#585858"));
        }
    } else {
        if (in_focus) {
            draw_stroke (cr, radius, 5, 0, center_x, center_y, 1.0, Gdk::RGBA ("#D9D9D9"));
            draw_stroke (cr, radius, 5, 0, center_x, center_y, get_progress (), Gdk::RGBA ("#1c71d8"));
        } else {
            draw_stroke (cr, radius, 5, 0, center_x, center_y, 1.0, Gdk::RGBA ("#EDEDED"));
            draw_stroke (cr, radius, 5, 0, center_x, center_y, get_progress (), Gdk::RGBA ("#A7A7A7"));
        }
    }

    // Draw the icon (if any)
    auto icon = get_icon ();
    if (icon) {
        cr->rectangle (0, 0, allocation.get_width (), allocation.get_height ());
        cr->clip ();
        int width = icon->get_width ();
        int height = icon->get_height ();
        Gdk::Cairo::set_source_pixbuf (cr, icon, (allocation.get_width () - width) / 2,
                                       (allocation.get_height () - height) / 2);
        cr->paint ();
    }

    cr->restore ();

    return false;
}

int
CircularProgressBar::calculate_radius () const
{
    return std::min (get_allocated_width () / 2, get_allocated_height () / 2) - 1;
}

void
CircularProgressBar::draw_stroke (const Cairo::RefPtr<Cairo::Context>& cr, int radius, int line_width, int position,
                                  double center_x, double center_y, double progress, const Gdk::RGBA& color) const
{
    cr->set_line_width (line_width);
    int delta = radius - position - line_width / 2;
    cr->arc (center_x, center_y, delta, 1.5 * M_PI, (1.5 + progress * 2) * M_PI);
    Gdk::Cairo::set_source_rgba (cr, color);
    cr->stroke ();
}

const Glib::RefPtr<Gdk::Pixbuf>&
CircularProgressBar::get_icon () const
{
    return m_icon;
}

const double&
CircularProgressBar::get_progress () const
{
    return m_progress;
}

void
CircularProgressBar::set_icon (const Glib::RefPtr<Gdk::Pixbuf>& icon)
{
    m_icon = icon;
    queue_draw ();
}

void
CircularProgressBar::set_progress (const double& progress)
{
    m_progress = clamp (progress, 0.0, 1.0); // TODO: Replace with std::clamp
    queue_draw ();
}
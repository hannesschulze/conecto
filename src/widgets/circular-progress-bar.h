/* circular-progress-bar.h
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
namespace Widgets {

/**
 * @brief A widget displaying a progress in a circular shape
 *
 * Based on Optimizer's implementation, which is originally based on vala-circular-progressbar
 * by phastmike
 */
class CircularProgressBar : public Gtk::Bin {
  public:
    /**
     * @brief Construct a new CircularProgressBar widget
     */
    CircularProgressBar ();
    ~CircularProgressBar () {}

    /** @brief The icon displayed in the center of the progress bar */
    const Glib::RefPtr<Gdk::Pixbuf>& get_icon () const;
    /** @brief The current progress displayed [0..1] */
    const double& get_progress () const;
    /** @brief The icon displayed in the center of the progress bar */
    void set_icon (const Glib::RefPtr<Gdk::Pixbuf>& icon);
    /** @brief The current progress displayed [0..1] */
    void set_progress (const double& progress);

    CircularProgressBar (const CircularProgressBar&) = delete;
    CircularProgressBar& operator= (const CircularProgressBar&) = delete;

  protected:
    // Overrides
    Gtk::SizeRequestMode get_request_mode_vfunc () const override;
    void                 get_preferred_width_vfunc (int& min_width, int& natural_width) const override;
    void                 get_preferred_height_vfunc (int& min_height, int& natural_height) const override;
    bool                 on_draw (const Cairo::RefPtr<Cairo::Context>& cr) override;

  private:
    int  calculate_radius () const;
    void draw_stroke (const Cairo::RefPtr<Cairo::Context>& cr, int radius, int line_width, int position,
                      double center_x, double center_y, double progress, const Gdk::RGBA& color) const;

    Glib::RefPtr<Gdk::Pixbuf> m_icon;
    double                    m_progress;
};

} // namespace Widgets
} // namespace App
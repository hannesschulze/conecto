/* settings-manager.h
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
namespace Controllers {

/**
 * @brief The @p SettingsManager is responsible for managing gsettings values
 */
class SettingsManager : public Gio::Settings {
  public:
    /**
     * @brief Return a single instance of this class
     */
    static SettingsManager& get_instance ()
    {
        static SettingsManager instance;
        return instance;
    }
    ~SettingsManager () {}

    /** @brief This property will represent the location x of the screen. */
    int get_window_x () const;
    /** @brief This property will represent the location x of the screen. */
    void set_window_x (int x);

    /** @brief This property will represent the location y of the screen. */
    int get_window_y () const;
    /** @brief This property will represent the location y of the screen. */
    void set_window_y (int y);

    /** @brief This property will represent the width of the application window. */
    int get_window_width () const;
    /** @brief This property will represent the width of the application window. */
    void set_window_width (int width);

    /** @brief This property will represent the height of the application window. */
    int get_window_height () const;
    /** @brief This property will represent the height of the application window. */
    void set_window_height (int height);

    SettingsManager (const SettingsManager&) = delete;
    SettingsManager& operator= (const SettingsManager&) = delete;

  private:
    SettingsManager ();
};

} // namespace Controllers
} // namespace App

#define SETTINGS (App::Controllers::SettingsManager::get_instance ())
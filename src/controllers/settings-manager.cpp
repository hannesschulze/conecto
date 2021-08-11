/* settings-manager.cpp
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

#include "settings-manager.h"
#include <conecto.h>

using namespace App::Controllers;

SettingsManager::SettingsManager ()
    : Gio::Settings (Conecto::Constants::APP_ID)
{
}

int
SettingsManager::get_window_x () const
{
    return get_int ("window-x");
}

void
SettingsManager::set_window_x (int x)
{
    set_int ("window-x", x);
}

int
SettingsManager::get_window_y () const
{
    return get_int ("window-y");
}

void
SettingsManager::set_window_y (int y)
{
    set_int ("window-y", y);
}

int
SettingsManager::get_window_width () const
{
    return get_int ("window-width");
}

void
SettingsManager::set_window_width (int width)
{
    set_int ("window-width", width);
}

int
SettingsManager::get_window_height () const
{
    return get_int ("window-height");
}

void
SettingsManager::set_window_height (int height)
{
    set_int ("window-height", height);
}

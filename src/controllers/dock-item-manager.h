/* dock-item-manager.h
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
#include "../models/connected-devices.h"
#include "../models/unavailable-devices.h"

namespace App {
namespace Controllers {

/**
 * @brief The @p DockItemManager is responsible for managing the starred items in Plank (if installed)
 * 
 * This class listens for both dock changes and model changes and updates both of them respectively
 */
class DockItemManager {
  public:
    /**
     * @brief Return a single instance of this class
     */
    static DockItemManager&
    get_instance ()
    {
        static DockItemManager instance;
        return instance;
    }
    ~DockItemManager () {}

    /** @brief Update the models to look in */
    void set_models (const Glib::RefPtr<Models::ConnectedDevices>& connected_devices,
                     const Glib::RefPtr<Models::UnavailableDevices>& unavailable_devices);

    DockItemManager (const DockItemManager&) = delete;
    DockItemManager& operator= (const DockItemManager&) = delete;

  private:
    DockItemManager ();

    Glib::RefPtr<Models::ConnectedDevices> m_connected_devices;
    Glib::RefPtr<Models::UnavailableDevices> m_unavailable_devices;

    void sync ();
};

} // namespace Controllers
} // namespace App

#define DOCK_ITEMS (App::Controllers::DockItemManager::get_instance ())
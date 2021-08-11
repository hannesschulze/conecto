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
    static DockItemManager& get_instance ()
    {
        static DockItemManager instance;
        return instance;
    }
    ~DockItemManager () {}

    /** @brief Update the models to look in */
    void set_models (const Glib::RefPtr<Models::ConnectedDevices>&   connected_devices,
                     const Glib::RefPtr<Models::UnavailableDevices>& unavailable_devices);

    /** @brief Start listening for changes (if not done already) */
    void listen ();

    /** @brief Manually update dock items (usually not necessary when using listen) */
    void update (std::function<void ()>&& cb);

    /** @brief Get the item position for a device selected from the dock */
    void get_position_for_id (const std::string&                                                            id,
                              std::function<void (int /* x */, int /* y */, Gtk::PositionType /* pos */)>&& cb);

    DockItemManager (const DockItemManager&) = delete;
    DockItemManager& operator= (const DockItemManager&) = delete;

  private:
    DockItemManager ();

    struct DeviceInfo {
        DeviceInfo (const Glib::ustring& id, const Glib::ustring& name, const Glib::ustring& icon_name)
            : id (id)
            , name (name)
            , icon_name (icon_name)
        {
        }
        Glib::ustring id, name, icon_name;
    };

    /** @brief Find a (starred) device by id, or return nullptr */
    std::shared_ptr<DeviceInfo> find_starred (const std::string& id) const;
    /** @brief Get a list of (starred) devices */
    std::list<std::shared_ptr<DeviceInfo>> get_starred () const;
    /** @brief Dirty hack to prevent removal if moved between the two models */
    void update_timeout ();

    using DockAction = std::function<void ()>;

    /** @brief Schedule an action for when the Dbus interface becomes available */
    void schedule_action (const DockAction& action);

    Glib::RefPtr<Models::ConnectedDevices>   m_connected_devices;
    Glib::RefPtr<Models::UnavailableDevices> m_unavailable_devices;

    bool m_is_listening;

    void sync ();
};

} // namespace Controllers
} // namespace App

#define DOCK_ITEMS (App::Controllers::DockItemManager::get_instance ())
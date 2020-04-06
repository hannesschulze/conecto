/* active-device-manager.h
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

#include <conecto.h>
#include "../models/connected-devices.h"
#include "../models/unavailable-devices.h"
#include "../models/available-devices.h"

namespace App {
namespace Controllers {

/**
 * @brief The @p ActiveDeviceManager keeps track of the currently activated device
 */
class ActiveDeviceManager {
  public:
    /**
     * @brief Return a single instance of this class
     */
    static ActiveDeviceManager&
    get_instance ()
    {
        static ActiveDeviceManager instance;
        return instance;
    }
    ~ActiveDeviceManager () {}

    /** Update the models to look in */
    void set_models (const Glib::RefPtr<Models::ConnectedDevices>& connected_devices,
                     const Glib::RefPtr<Models::UnavailableDevices>& unavailable_devices,
                     const Glib::RefPtr<Models::AvailableDevices>& available_devices);

    /**
     * @brief Activate a new device and find it in one of the models
     * 
     * If not found, the currently active device will be deselected
     * 
     * @param device The new device
     */
    void activate_device (const std::shared_ptr<Conecto::Device>& device);

    /**
     * @brief Get the currently activated device (may be empty)
     */
    std::shared_ptr<Conecto::Device> get_device () const noexcept;

    using type_signal_device_update = sigc::signal<void, const Gtk::TreeIter& /* it */, bool /* new_device */>;
    /**
     * @param it The iterator inside of the connected_devices model
     * @param new_device true if the new device recently was in another model
     */
    using type_signal_connected_device_update = type_signal_device_update;
    /**
     * @param it The iterator inside of the available_devices model
     * @param new_device true if the new device recently was in another model
     */
    using type_signal_available_device_update = type_signal_device_update;
    /**
     * @param it The iterator inside of the unavailable_devices model
     * @param new_device true if the new device recently was in another model
     */
    using type_signal_unavailable_device_update = type_signal_device_update;
    using type_signal_no_device_selected = sigc::signal<void>;
    /**
     * Emitted when the active device has been updated to an entry from the connected_devices model
     */
    type_signal_connected_device_update signal_connected_device_update () { return m_signal_connected_device_update; }
    /**
     * Emitted when the active device has been updated to an entry from the available_devices model
     */
    type_signal_available_device_update signal_available_device_update () { return m_signal_available_device_update; }
    /**
     * Emitted when the active device has been updated to an entry from the unavailable_devices model
     */
    type_signal_unavailable_device_update signal_unavailable_device_update () { return m_signal_unavailable_device_update; }
    /**
     * Emitted when the active device has been reset
     */
    type_signal_no_device_selected signal_no_device_selected () { return m_signal_no_device_selected; }

    ActiveDeviceManager (const ActiveDeviceManager&) = delete;
    ActiveDeviceManager& operator= (const ActiveDeviceManager&) = delete;

  private:
    ActiveDeviceManager ();

    std::shared_ptr<Conecto::Device> m_device;
    sigc::connection                 m_connection_changed;
    sigc::connection                 m_connection_removed;

    Glib::RefPtr<Models::ConnectedDevices> m_connected_devices;
    Glib::RefPtr<Models::UnavailableDevices> m_unavailable_devices;
    Glib::RefPtr<Models::AvailableDevices> m_available_devices;

    type_signal_connected_device_update m_signal_connected_device_update;
    type_signal_available_device_update m_signal_available_device_update;
    type_signal_unavailable_device_update m_signal_unavailable_device_update;
    type_signal_no_device_selected m_signal_no_device_selected;

    template<class T>
    void connect_signals (const T& model, type_signal_device_update& signal)
    {
        m_connection_changed = model->signal_row_changed ().connect
            ([this, &signal, model](const Gtk::TreeModel::Path& path, const Gtk::TreeModel::iterator& it) {
            if (model->get_device (it) == m_device)
                signal.emit (it, false);
        });
        m_connection_removed = model->signal_row_deleted ().connect ([this, model](const Gtk::TreeModel::Path& path) {
            if (!model->find_device (m_device))
                activate_device (std::shared_ptr<Conecto::Device> ());
        });
    }
};

} // namespace Controllers
} // namespace App

#define ACTIVE_DEVICE (App::Controllers::ActiveDeviceManager::get_instance ())
/* notifications.h
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

#include "abstract-packet-handler.h"
#include <map>
#include <sigc++/sigc++.h>
#include <glibmm/datetime.h>
#include <libnotify/notification.h>

namespace Conecto {

class NetworkPacket;

namespace Plugins {

/**
 * @brief Notifications plugin
 */
class Notifications : public AbstractPacketHandler {
  public:
    /**
     * Create a new instance of this plugin
     */
    Notifications ();
    ~Notifications () {}

    struct NotificationInfo {
        std::string    id;
        std::string    app_name;
        std::string    title;
        std::string    body;
        Glib::DateTime time;
    };

    /**
     * @param device The device which sent the notification
     * @param notification The notification
     */
    using type_signal_new_notification =
            sigc::signal<void, const std::shared_ptr<Device>& /* device */, const NotificationInfo& /* notification */>;
    /**
     * @param device The device
     * @param id The dismissed notification's id
     */
    using type_signal_notification_dismissed =
            sigc::signal<void, const std::shared_ptr<Device>& /* device */, const std::string& /* id */>;
    /**
     * Emitted after receiving a new notification from a device
     */
    type_signal_new_notification signal_new_notification () { return m_signal_new_notification; }
    /**
     * Emitted after a notification has been missed - either by us or by the peer
     */
    type_signal_notification_dismissed signal_notification_dismissed () { return m_signal_notification_dismissed; }

    /** @brief Dismiss a notification, this will immediately emit a @p signal_notification_dismissed */
    void dismiss (const std::shared_ptr<Device>& device, const std::string& id);

    Notifications (const Notifications&) = delete;
    Notifications& operator= (const Notifications&) = delete;

  protected:
    // packet handler
    void on_message (const NetworkPacket& message, const std::shared_ptr<Device>& device);

    // overrides
    std::string get_packet_type_virt () const noexcept override;
    void        register_device_virt (const std::shared_ptr<Device>& device) noexcept override;
    void        unregister_device_virt (const std::shared_ptr<Device>& device) noexcept override;

    // default signal handlers
    void on_new_notification (const std::shared_ptr<Device>& device, const NotificationInfo& notification);
    void on_notification_dismissed (const std::shared_ptr<Device>& device, const std::string& id);

  private:
    std::map<std::shared_ptr<Device>, sigc::connection> m_devices;

    type_signal_new_notification       m_signal_new_notification;
    type_signal_notification_dismissed m_signal_notification_dismissed;

    static void on_notification_closed (NotifyNotification* notification, Notifications* self);

    std::map<std::string /* id */, std::shared_ptr<NotifyNotification> /* notification */> m_desktop_notifications;
};

} // namespace Plugins
} // namespace Conecto
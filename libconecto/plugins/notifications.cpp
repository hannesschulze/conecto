/* notifications.cpp
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

#include "notifications.h"
#include "device.h"
#include "network-packet.h"

using namespace Conecto::Plugins;

namespace {

constexpr char PACKET_TYPE[] = "kdeconnect.notification";

} // namespace

Notifications::Notifications ()
    : AbstractPacketHandler ()
{
    m_signal_new_notification.connect (sigc::mem_fun (*this, &Notifications::on_new_notification));
    m_signal_notification_dismissed.connect (sigc::mem_fun (*this, &Notifications::on_notification_dismissed));
}

std::string
Notifications::get_packet_type_virt () const noexcept
{
    return PACKET_TYPE;
}

void
Notifications::register_device_virt (const std::shared_ptr<Device>& device) noexcept
{
    m_devices[device] =
            device->signal_message ().connect (sigc::bind (sigc::mem_fun (*this, &Notifications::on_message), device));
}

void
Notifications::unregister_device_virt (const std::shared_ptr<Device>& device) noexcept
{
    if (m_devices.find (device) != m_devices.end ()) {
        m_devices.at (device).disconnect ();
        m_devices.erase (device);
    }
}

void
Notifications::on_message (const NetworkPacket& message, const std::shared_ptr<Device>& device)
{
    if (message.get_type () != PACKET_TYPE) return;
    const auto& json = message.get_body ();

    // a unique id for the notification
    std::string id = json["id"].asString ();

    if (json["isCancel"].isBool () && json["isCancel"].asBool ()) {
        // notification was dismissed
        m_signal_notification_dismissed (device, id);
        return;
    }

    if (!json["appName"].isString () || !json["ticker"].isString ()) return;

    Glib::DateTime time = Glib::DateTime::create_now_local ();
    if (json["time"].isString ()) {
        long timestamp = atol (json["time"].asCString ());
        timestamp /= 1000;
        time = Glib::wrap (g_date_time_new_from_unix_local (timestamp));
    }

    std::string body = json["text"].isString () ? json["text"].asString () : std::string ();

    NotificationInfo notification = { .id = id,
                                      .app_name = json["appName"].asString (),
                                      .title = json["ticker"].asString (),
                                      .body = json["text"].asString (),
                                      .time = time };

    m_signal_new_notification.emit (device, notification);
}

void
Notifications::on_new_notification (const std::shared_ptr<Device>& device, const NotificationInfo& notification)
{
    if (m_desktop_notifications.find (notification.id) != m_desktop_notifications.end ()) return;

    std::shared_ptr<NotifyNotification> desktop_notification (notify_notification_new (notification.app_name.c_str (),
                                                                                       notification.title.c_str (),
                                                                                       "phone"),
                                                              g_object_unref);

    g_signal_connect (desktop_notification.get (), "closed", G_CALLBACK (on_notification_closed), this);
    GError* err = nullptr;
    notify_notification_show (desktop_notification.get (), &err);
    if (err)
        g_error_free (err);
    else
        m_desktop_notifications.insert ({ notification.id, desktop_notification });
}

void
Notifications::on_notification_dismissed (const std::shared_ptr<Device>& device, const std::string& id)
{
    if (m_desktop_notifications.find (id) == m_desktop_notifications.end ()) return;

    GError* err = nullptr;
    notify_notification_close (m_desktop_notifications.at (id).get (), &err);
    if (err)
        g_error_free (err);
    else
        m_desktop_notifications.erase (id);
}

void
Notifications::on_notification_closed (NotifyNotification* notification, Notifications* self)
{
    for (const auto& item : self->m_desktop_notifications) {
        if (item.second.get () == notification) {
            self->m_desktop_notifications.erase (item.first);
            return;
        }
    }
}

void
Notifications::dismiss (const std::shared_ptr<Device>& device, const std::string& id)
{
    Json::Value body (Json::objectValue);
    body["cancel"] = id;
    NetworkPacket packet ("kdeconnect.notification.request", body);
    device->send (packet);
    m_signal_notification_dismissed.emit (device, id);
}
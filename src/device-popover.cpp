/* device-popover.cpp
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

#include "device-popover.h"
#include "controllers/active-device-manager.h"
#ifdef ENABLE_PLANK_SUPPORT
#include "controllers/dock-item-manager.h"
#endif

using namespace App;

DevicePopover::DevicePopover (const Glib::RefPtr<Models::ConnectedDevices>& connected_devices,
                              const std::shared_ptr<Models::SMSStorage>&    sms_storage,
                              const Glib::ustring& id)
    : Widgets::PopoverWindow ()
    , m_active_view (id, connected_devices, sms_storage)
{
    set_size_request (600, 430);
    set_default_size (600, 430);
    add (m_active_view);
    m_active_view.show_all ();

    m_active_view.signal_close_popover ().connect (sigc::mem_fun (*this, &DevicePopover::close_popover));

    // Set up CSS
    auto main_provider = Gtk::CssProvider::create ();
    auto fallback_provider = Gtk::CssProvider::create ();
    main_provider->load_from_resource ("/com/github/hannesschulze/conecto/css/style.css");
    fallback_provider->load_from_resource ("/com/github/hannesschulze/conecto/css/fallback.css");
    Gtk::StyleContext::add_provider_for_screen (Gdk::Screen::get_default (), main_provider,
                                                GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    Gtk::StyleContext::add_provider_for_screen (Gdk::Screen::get_default (), fallback_provider,
                                                GTK_STYLE_PROVIDER_PRIORITY_FALLBACK);

    ACTIVE_DEVICE.activate_device (id);
}

std::shared_ptr<DevicePopover>
DevicePopover::create (Gtk::Application& app, const Glib::RefPtr<Models::ConnectedDevices>& connected_devices,
                       const std::shared_ptr<Models::SMSStorage>& sms_storage,
                       const Glib::ustring& id)
{
    std::shared_ptr<DevicePopover> res (new DevicePopover (connected_devices, sms_storage, id));
    app.add_window (*res);
#ifdef ENABLE_PLANK_SUPPORT
    DOCK_ITEMS.get_position_for_id (id, [res] (int x, int y, Gtk::PositionType pos) {
        res->set_pointing_to (x, y, pos);
        res->show_all ();
    });
#else
    res->set_pointing_to (0, 0, Gtk::POS_TOP);
    res->show_all ();
#endif
    return res;
}
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

DevicePopover::DevicePopover (const std::string& id)
    : Widgets::PopoverWindow ()
{
    ACTIVE_DEVICE.activate_device (id);
    Gtk::Label* lbl = Gtk::make_managed<Gtk::Label> ("Device: " + id);
    set_size_request (500, 370);
    add (*lbl);
    lbl->show_all ();
}

std::shared_ptr<DevicePopover>
DevicePopover::create (Gtk::Application& app, const std::string& id)
{
    std::shared_ptr<DevicePopover> res (new DevicePopover (id));
    app.add_window (*res);
#ifdef ENABLE_PLANK_SUPPORT
    DOCK_ITEMS.get_position_for_id (id, [res](int x, int y, Gtk::PositionType pos) {
        res->set_pointing_to (x, y, pos);
        res->show_all ();
    });
#else
    res->set_pointing_to (0, 0, Gtk::POS_TOP);
    res->show_all ();
#endif
    return res;
}
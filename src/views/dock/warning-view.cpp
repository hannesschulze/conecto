/* warning-view.cpp
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

#include "warning-view.h"
#include "../../controllers/active-device-manager.h"
#include "../../controllers/dock-item-manager.h"
#include <conecto.h>

using namespace App::Views::Dock;

WarningView::WarningView (BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder> glade_ref)
    : Gtk::Grid (cobject)
    , m_builder (glade_ref)
    , m_btn_remove (nullptr)
{
    m_builder->get_widget ("btn_remove", m_btn_remove);

    m_btn_remove->signal_clicked ().connect (sigc::mem_fun (*this, &WarningView::on_btn_remove_clicked));
}

std::shared_ptr<WarningView>
WarningView::create (const Glib::ustring& id)
{
    WarningView* res = nullptr;
    auto         builder =
            Gtk::Builder::create_from_resource ("/com/github/hannesschulze/conecto/ui/views/dock/warning-view.ui");
    builder->get_widget_derived ("ConectoViewsDockWarningView", res);
    res->m_id = id;
    return std::shared_ptr<WarningView> (res);
}

void
WarningView::on_btn_remove_clicked ()
{
    auto dev = ACTIVE_DEVICE.get_device ();

    if (dev) {
        Conecto::Backend::get_instance ().get_config ().set_device_starred (dev, false);
        ACTIVE_DEVICE.activate_device (std::shared_ptr<Conecto::Device> ());
    }
#ifdef ENABLE_PLANK_SUPPORT
    DOCK_ITEMS.update ([this] () { m_signal_close_popover.emit (); });
#endif
}
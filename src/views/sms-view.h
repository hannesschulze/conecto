/* sms-view.h
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
#include <granite.h>
#include "../models/sms-storage.h"

namespace App {
namespace Views {

/**
 * @brief A view for listing received messages and sending SMS messages
 *
 * Connected to the following model: @p App::Models::SMSStorage
 */
class SMSView : public Gtk::Bin {
  public:
    /**
     * @brief Create an SMS view
     */
    static std::shared_ptr<SMSView> create (const std::shared_ptr<Models::SMSStorage>& model);
    ~SMSView () {}

    /**
     * @brief Set the current device
     */
    void set_device (const std::shared_ptr<Conecto::Device>& device);

    SMSView (const SMSView&) = delete;
    SMSView& operator= (const SMSView&) = delete;

  private:
    SMSView (const std::shared_ptr<Models::SMSStorage>& model);

    void create_placeholder_tab ();

    void on_new_tab_requested ();
    bool on_close_tab_requested (GraniteWidgetsTab* tab);
    static void static_on_new_tab_requested (GraniteWidgetsDynamicNotebook* sender, SMSView* self);
    static gboolean static_on_close_tab_requested (GraniteWidgetsDynamicNotebook* sender, GraniteWidgetsTab* tab,
                                                   SMSView* self);

    std::shared_ptr<Models::SMSStorage>            m_model;
    std::shared_ptr<GraniteWidgetsDynamicNotebook> m_notebook;
    std::shared_ptr<Gtk::Box>                      m_placeholder;
    std::shared_ptr<GraniteWidgetsTab>             m_placeholder_tab;
    std::shared_ptr<Conecto::Device>               m_device;

    std::list<std::shared_ptr<Gtk::Widget>> m_widget_owners;
};

} // namespace Views
} // namespace App
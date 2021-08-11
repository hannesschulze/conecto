/* mouse.h
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
#include <gdkmm/display.h>

namespace Conecto {

class NetworkPacket;

namespace Plugins {

/**
 * @brief Mouse plugin
 */
class Mouse : public AbstractPacketHandler {
  public:
    /**
     * Create a new instance of this plugin
     */
    Mouse ();
    ~Mouse ();

    Mouse (const Mouse&) = delete;
    Mouse& operator= (const Mouse&) = delete;

  protected:
    // packet handler
    void on_message (const NetworkPacket& message, const std::shared_ptr<Device>& device);

    // overrides
    std::string get_packet_type_virt () const noexcept override;
    void        register_device_virt (const std::shared_ptr<Device>& device) noexcept override;
    void        unregister_device_virt (const std::shared_ptr<Device>& device) noexcept override;

  private:
    std::map<std::shared_ptr<Device>, sigc::connection> m_devices;
    Glib::RefPtr<Gdk::Display>                          m_display;

    void send_click (int button, bool doubleclick = false);
};

} // namespace Plugins
} // namespace Conecto
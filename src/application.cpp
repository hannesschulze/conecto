/* application.cpp
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

#include "application.h"
#include <conecto.h>
#include "models/connected-devices.h"
#include "models/unavailable-devices.h"
#include "controllers/active-device-manager.h"
#include "device-popover.h"
#ifdef ENABLE_PLANK_SUPPORT
#include "controllers/dock-item-manager.h"
#endif

using namespace App;

Application::Application ()
    : Gtk::Application (Conecto::Constants::APP_ID, Gio::APPLICATION_HANDLES_COMMAND_LINE)
    , m_connected_devices (Models::ConnectedDevices::create ())
    , m_unavailable_devices (Models::UnavailableDevices::create ())
    , m_available_devices (Models::AvailableDevices::create ())
{
}

Glib::RefPtr<Application>
Application::create ()
{
    return Glib::RefPtr<Application> (new Application);
}

void
Application::on_startup ()
{
    Gtk::Application::on_startup ();

    // Register non-interactive plugins
    Conecto::Backend::get_instance ().register_plugin (std::make_shared<Conecto::Plugins::Mouse> ());

    Conecto::Backend::get_instance ().load_from_cache ();
    Conecto::Backend::get_instance ().listen ();
    ACTIVE_DEVICE.set_models (m_connected_devices, m_unavailable_devices, m_available_devices);
#ifdef ENABLE_PLANK_SUPPORT
    DOCK_ITEMS.set_models (m_connected_devices, m_unavailable_devices);
#endif
}

void
Application::on_activate ()
{
    if (m_open_dev_id == Glib::ustring ()) {
        // Get the current window, create one if it doesn't exist
        Gtk::Window* window = get_active_window ();

        if (!window) {
            m_window = Window::create (m_connected_devices, m_unavailable_devices, m_available_devices);
            add_window (*m_window);
            window = m_window.get ();
        }

        window->present ();
    } else {
        // Open a new popover-like window
        auto popover = DevicePopover::create (*this, m_connected_devices, m_open_dev_id);
        m_popovers.push_back (popover);
    }
}

int
Application::on_command_line (const Glib::RefPtr<Gio::ApplicationCommandLine>& command_line)
{
    Gtk::Application::on_command_line (command_line);

    hold ();

    Glib::OptionContext context ("- Conecto");
    Glib::OptionGroup   group ("Options", "");
    Glib::OptionEntry   open_dev_option;
    open_dev_option.set_long_name ("open-dev");
    open_dev_option.set_short_name ('o');
    group.add_entry (open_dev_option, m_open_dev_id);
    context.set_main_group (group);
    context.set_help_enabled (true);
    int    argc;
    char** argv = command_line->get_arguments (argc);
    if (!context.parse (argv)) {
        g_error ("Error while trying to parse command line arguments");
        return 1;
    }

    activate ();

    release ();
    return 0;
}
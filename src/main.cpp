/* main.cpp
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

#include <conecto.h>
#include <giomm/init.h>
#include <giomm/application.h>
#include <iostream>

namespace {

void
on_activate (Glib::RefPtr<Gio::Application> app)
{
    auto& backend = Conecto::Backend::get_instance ();

    auto ping = std::make_shared<Conecto::Plugins::Ping> ();
    ping->signal_ping ().connect ([](const std::shared_ptr<Conecto::Device>& dev) {
        std::cerr << "Received a ping from " << dev->to_string () << std::endl;
    });
    backend.register_plugin (ping);

    backend.signal_found_new_device ().connect ([](const std::shared_ptr<Conecto::Device>& dev) {
        std::cerr << "Found device: " << dev->to_string () << std::endl;

        dev->signal_connected ().connect ([dev]() {
            std::cerr << "Connected: " << dev->to_string () << std::endl;
        });

        dev->signal_disconnected ().connect ([dev]() {
            std::cerr << "Disconnected: " << dev->to_string () << std::endl;
        });

        dev->signal_paired ().connect ([dev](bool success) {
            std::cerr << "Paired (" << std::boolalpha << success << "): " << dev->to_string () << std::endl;
        });
    });
    backend.load_from_cache ();
    backend.listen ();
    app->hold ();
}

} // namespace

int
main (int argc, char** argv)
{
    Gio::init ();

    Glib::RefPtr<Gio::Application> app = Gio::Application::create ("com.github.hannesschulze.conecto");

    app->signal_activate ().connect (sigc::bind (&on_activate, app));

    return app->run (argc, argv);
}
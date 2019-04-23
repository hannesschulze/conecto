/**
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * AUTHORS
 * gyan000 <gyan000 (at] ijaz.fr>
 */
using Conecto.Widgets;
using Conecto.Plugin;
using MConnect;

namespace Conecto.Plugin {

    public class Clipboard : PluginInterface {

        construct {
            name  = _("Clipboard");
            icon_name = "edit-copy"; // TODO clipboard icon
            capability_name = ClipboardHandler.CLIPBOARD;
            description = _("Share clipboard");
            settings_key_is_active = "kdeconnect-clipboard-active";
            show_configure_button = false;
        }
    }
}

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
using EOSConnect.Plugin.Popover;
using EOSConnect.Plugin.Dialogs;
using EOSConnect.Widgets;
using Gee;
using MConnect;

namespace EOSConnect.Plugin {

    public class Share : PluginInterface {

        private ShareSettings share_settings_popover;

        construct {
            name  = _("Share");
            icon_name = "emblem-shared";
            capability_name = ShareHandler.SHARE_PKT;
            description = _("Share files trough devices.");
            settings_key_is_active = "kdeconnect-share-active";
            show_configure_button = true;
        }

        public override void handle_configure_button (Device device) {
            if (share_settings_popover == null) {
                var share_settings_popover = new ShareSettings (device, row_configure_button);
                share_settings_popover.show_all ();

                share_settings_popover.destroy.connect (() => {
                    share_settings_popover = null;
                });
            }
        }

        public void select_files (Device device) {
            Gtk.FileChooserDialog chooser = new Gtk.FileChooserDialog (
                "Select your favorite file", parent_window, Gtk.FileChooserAction.OPEN,
                "_Cancel", Gtk.ResponseType.CANCEL,
                "_Open", Gtk.ResponseType.ACCEPT);

            chooser.select_multiple = true;

            if (chooser.run () == Gtk.ResponseType.ACCEPT) {

                send_files (device, chooser.get_uris ());
            }

            chooser.close ();
        }

        public void send_files (Device device, SList<string> uris) {

            ShareFileInfoProgress file_info_progress =  new ShareFileInfoProgress (uris, parent_window);
            file_info_progress.show_all ();

            int current_file_num = 0;
            foreach (unowned string uri in uris) {
                try {
                    UploadTransfer upload_transfer;

                    ((ShareHandler)device.get_path_capability_handler (ShareHandler.SHARE))
                        .share_file (device, uri, ++current_file_num, out upload_transfer);

                    // TOCHECK - potential memory leak here ?
                    upload_transfer.progress.connect ((progress_percentage, file_num, file_uri) => {
                        file_info_progress.update_progress (--file_num, progress_percentage);
                    });
                } catch (Error e) {
                    file_info_progress.destroy ();
                    warning ("Error: %s", e.message);
                }
            }
        }
    }
}

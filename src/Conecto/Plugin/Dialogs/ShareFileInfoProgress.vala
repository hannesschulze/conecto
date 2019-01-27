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
using Conecto;

namespace Conecto.Plugin.Dialogs {

    public class ShareFileInfoProgress : Gtk.Dialog {

        private Gtk.ListBox listbox_files;
        public unowned SList<string> uris { get; construct; }

        public ShareFileInfoProgress (SList<string> uris, Gtk.Window? window_parent) {
            Object (
               border_width: 5,
               deletable: false,
               resizable: true,
               title: _("File uploads"),
               transient_for: window_parent,
               uris: uris
           );
        }

        construct {
            width_request = 500;
            height_request = 350;
            listbox_files = new Gtk.ListBox ();

            var scrolled_window = new Gtk.ScrolledWindow (null, null);
            scrolled_window.expand = true;
            scrolled_window.hscrollbar_policy = Gtk.PolicyType.NEVER;
            scrolled_window.add (listbox_files);

            int num_file = 0;
            foreach (unowned string uri in uris) {
                listbox_files.insert (
                    new ProgressFileListBoxRow (uri), ++num_file
                );
            }

            get_content_area ().add (scrolled_window);

            add_button ("C_ancel", Gtk.ResponseType.CANCEL);
            add_button ("_Close", Gtk.ResponseType.CLOSE);

            response.connect (on_response);

            show_all ();
        }



        public void update_progress (int file_num, double progress_percentage) {

            ProgressFileListBoxRow box_row = (ProgressFileListBoxRow)listbox_files.get_row_at_index (file_num);
            if(box_row == null) {
                return;
            }

            if (progress_percentage < 0) {
                box_row.progress_bar.pulse ();
            }
            else {
                box_row.progress_bar.set_fraction (progress_percentage);
            }
        }

        private void on_response (Gtk.Dialog source, int response_id) {
            switch (response_id) {
                case Gtk.ResponseType.CLOSE:
                    destroy ();
                    break;

                case Gtk.ResponseType.CANCEL:
                    // TODO - implement upload cancel.
                    warning ("Cancel operation not yet implemented.");
                    destroy ();
                    break;
            }
    	}

        private class ProgressFileListBoxRow : Gtk.ListBoxRow {

            public Gtk.ProgressBar progress_bar;
            public ProgressFileListBoxRow (string uri) {

                var vbox = new Gtk.Box (Gtk.Orientation.VERTICAL, 0);
                vbox.margin = 5;

                progress_bar = new Gtk.ProgressBar ();
                progress_bar.pulse_step = 0.05;
                vbox.pack_start (progress_bar, true, false, 0);

                var details = new Gtk.Label (File.new_for_uri (uri).get_path ());
                details.set_line_wrap (true);
                details.set_alignment ((float) 0.0, (float) 0.5);
                vbox.pack_start (details, true, false, 0);

                add (vbox);
            }
        }
    }
}

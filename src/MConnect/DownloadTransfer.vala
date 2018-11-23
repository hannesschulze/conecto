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
 * Maciek Borzecki <maciek.borzecki (at] gmail.com>
 * gyan000 <gyan000(at] ijaz.fr>
 */

namespace MConnect {

    public class DownloadTransfer : TransferInterface, Object {

        private Cancellable cancellable = null;
        private Device device = null;
        private File file = null;
        private FileOutputStream file_output_stream = null;
        private InetSocketAddress isa = null;
        private IOCopyJob job = null;
        private SocketConnection socket_connection = null;
        private TlsConnection tls_connection = null;
        private Unity.LauncherEntry launcher_entry;

        public uint64 size = 0;
        public string destination = "";

        public DownloadTransfer (Device device, InetSocketAddress isa, uint64 size, string destination) {
            this.launcher_entry = Unity.LauncherEntry.get_for_desktop_id (EOSConnect.App.GSETTINGS_SCHEMA_ID + ".desktop");
            this.isa = isa;
            this.cancellable = new Cancellable ();
            this.destination = destination;
            this.size = size;
            this.device = device;
        }

        public async bool start_async () {
            try {
                file = File.new_for_path (destination + ".part");
                file_output_stream = file.replace (null, false, FileCreateFlags.PRIVATE | FileCreateFlags.REPLACE_DESTINATION);
            } catch (Error e) {
                warning ("Failed to open destination path %s: %s", destination, e.message);
                return false;
            }

            debug ("Start transfer from %s:%u", isa.address.to_string (), isa.port);
            var client = new SocketClient ();

            try {
                socket_connection = yield client.connect_async (isa);

                debug ("Connected.");
            } catch (Error e) {
                var err = "Failed to connect: %s".printf (e.message);
                warning (err);
                cleanup_error (err);
                return false;
            }

            var sock = socket_connection.get_socket ();
            Utils.socket_set_keepalive (sock);

            // enable TLS
            try {
                tls_connection = Utils.make_tls_connection (socket_connection,
                                                            Core.instance ().certificate,
                                                            device.certificate,
                                                            Utils.TlsConnectionMode.CLIENT);

                debug ("Attempt TLS handshake.");
                //var tls_res = yield this.tls_connection.handshake_async ();
                yield this.tls_connection.handshake_async ();

                debug ("TLS handshake complete.");
            } catch (Error e) {
                var err = "TLS handshake failed: %s".printf (e.message);
                warning (err);
                cleanup_error (err);
                return false;
            }

            start_transfer ();
            return true;
        }

        private void start_transfer () {
            debug ("Connected, start transfer.");
            if(launcher_entry != null) {
                launcher_entry.progress_visible = true;
            }

            job = new IOCopyJob (this.tls_connection.input_stream, this.file_output_stream);
            job.progress.connect ((t, done) => {
                int percent = (int) (100.0 * ((double) done / (double) this.size));
                //debug ("progress: %s/%s %d%%", format_size (done), format_size (this.size), percent);
                if(launcher_entry != null) {
                    launcher_entry.progress = (double)percent / 100;
                }
            });

            started ();
            job.start_async.begin (cancellable, job_complete);
        }

        private void job_complete (Object ? obj, AsyncResult res) {
            info ("Transfer finished.");
            if(launcher_entry != null) {
                launcher_entry.progress_visible = false;
            }
            try {
                var rcvd_bytes = job.start_async.end (res);
                debug ("Transfer done, got %s", format_size (rcvd_bytes));
                cleanup_success ();
            } catch (Error err) {
                warning ("Fransfer failed: %s", err.message);
                cleanup_error (err.message);
            }
        }

        private void cleanup () {
            if (file_output_stream != null) {
                try {
                    file_output_stream.close ();
                } catch (IOError e) {
                    warning ("Failed to close file output: %s", e.message);
                }
            }

            if (tls_connection != null) {
                try {
                    tls_connection.close ();
                } catch (IOError e) {
                    warning ("Failed to close TLS connection: %s", e.message);
                }
            }

            if (socket_connection != null) {
                try {
                    socket_connection.close ();
                } catch (IOError e) {
                    warning ("Failed to close connection: %s", e.message);
                }
            }

            if(launcher_entry != null) {
                launcher_entry.progress_visible = false;
            }

            file = null;
            file_output_stream = null;
            socket_connection = null;
            tls_connection = null;
            job = null;
        }

        private void cleanup_error (string reason) {
            try {
                file.@delete ();
                cleanup ();
                error (reason);
            } catch (Error e) {
                warning ("Error: %s", e.message);
            }
        }

        private void cleanup_success () {
            try {
                var dest = File.new_for_path (destination);
                file.move (dest, FileCopyFlags.OVERWRITE);
                cleanup ();
                finished ();
            } catch (Error e) {
                var err = "Failed to rename temporary file %s to %s: %s".printf (file.get_path (),
                                                                                 destination,
                                                                                 e.message);
                warning (err);
                cleanup_error (err);
            }
        }

        public void cancel () {
            debug ("Cancel called.");
            cancellable.cancel ();
        }
    }
}

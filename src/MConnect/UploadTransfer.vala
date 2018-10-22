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
 * gyan000 <gyan000 (at] ijaz.fr>
 */
namespace MConnect {

    public class UploadTransfer : TransferInterface, Object {

        private const int WAIT_TIMEOUT = 30;

        private Cancellable cancellable = null;
        private Device device = null;
        private FileInputStream file_input_stream = null;
        public IOCopyJob job { get; private set; default = null; }
        private SocketConnection socket_connection = null;
        private SocketService socket_service = null;
        private TlsConnection tls_connection = null;
        private Unity.LauncherEntry launcher_entry;
        private uint timeout_source = 0;
        private uint64 file_size;
        private int file_num = 1;
        private string file_uri = "";

        public UploadTransfer (
            Device device,
            SocketService socket_service,
            FileInputStream file_input_stream,
            uint64 file_size,
            int file_num = 1,
            string file_uri = ""
        ) {
            this.launcher_entry = Unity.LauncherEntry.get_for_desktop_id (EOSConnect.App.GSETTINGS_SCHEMA_ID + ".desktop");
            this.socket_service = socket_service;
            this.cancellable = new Cancellable ();
            this.device = device;
            this.file_input_stream = file_input_stream;
            this.file_size = file_size;
            this.file_num = file_num;
            this.file_uri = file_uri;
        }

        public async bool start_async () {
            debug ("Start transfer to device %s", device.to_string ());
            socket_service.incoming.connect (client_connected);

            debug ("Wait for client.");
            timeout_source = Timeout.add_seconds (WAIT_TIMEOUT, wait_timeout);
            socket_service.start ();

            return true;
        }

        public void cancel () {
            debug ("Cancel called.");
            cancellable.cancel ();
        }

        private bool wait_timeout () {
            warning ("Timeout waiting for client.");
            socket_service.stop ();
            cleanup_error ("Timeout waiting for client.");
            return false;
        }

        private bool client_connected (SocketConnection socket_connection, Object ? source) {
            if (timeout_source != 0) {
                Source.remove (timeout_source);
                timeout_source = 0;
            }

            handle_client.begin (socket_connection);
            return false;
        }

        private async void handle_client (SocketConnection socket_connection) {
            try {
                var isa = socket_connection.get_remote_address () as InetSocketAddress;
                debug ("Client connected: %s:%u", isa.address.to_string (), isa.port);
                this.socket_connection = socket_connection;

                var socket = this.socket_connection.get_socket ();
                Utils.socket_set_keepalive (socket);

                // enable TLS
                tls_connection = Utils.make_tls_connection (this.socket_connection,
                                                            Core.instance ().certificate,
                                                            device.certificate,
                                                            Utils.TlsConnectionMode.SERVER);

                debug ("Attempt TLS handshake.");
                var tls_res = yield tls_connection.handshake_async ();
                debug ("TLS handshake complete.");
            } catch (Error e) {
                var error = "TLS handshake failed: %s".printf (e.message);
                warning ("Error: %s", error);
                cleanup_error (error);
                return;
            }

            start_transfer ();
        }

        private void start_transfer () {
            debug ("Connected, start transfer.");
            if(launcher_entry != null) {
                launcher_entry.progress_visible = true;
            }

            job = new IOCopyJob (file_input_stream, tls_connection.output_stream);
            job.progress.connect ((t, bytes_done) => {
                int percent = (int) (100.0 * ((double) bytes_done / (double)file_size));
                //debug ("Progress: %s/%s %d%%", format_size (bytes_done), format_size (file_size), percent);
                if(launcher_entry != null) {
                    launcher_entry.progress = (double)percent / 100;
                }
                progress ((double)percent / 100, file_num, file_uri);
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
                warning ("Transfer failed: %s", err.message);

                cleanup_error (err.message);
            }
        }

        private void cleanup () {
            if (file_input_stream != null) {
                try {
                    file_input_stream.close ();
                } catch (IOError e) {
                    warning ("Failed to close file input: %s",  e.message);
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

            socket_service.stop ();
            socket_service.close ();
            file_input_stream = null;
            socket_connection = null;
            tls_connection = null;
            job = null;
        }

        private void cleanup_error (string reason) {
            cleanup ();
            error (reason);
        }

        private void cleanup_success () {
            cleanup ();
            finished ();
        }
    }
}

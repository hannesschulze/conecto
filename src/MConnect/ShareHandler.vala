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

    public class ShareHandler : Object, PacketHandlerInterface {

        public const string SHARE = "kdeconnect.share.request";
        public const string SHARE_PKT = "kdeconnect.share";
        public static string DOWNLOADS = null;
        public static string DOWNLOADS_TMP = "/tmp";

        public void use_device (Device dev) {
            debug ("Use device %s for sharing", dev.to_string ());
            dev.message.connect (this.message);
        }

        private ShareHandler () {
        }

        public static ShareHandler instance () {
            if (ShareHandler.DOWNLOADS == null) {

                ShareHandler.DOWNLOADS = Path.build_filename (
                    Environment.get_user_special_dir (UserDirectory.DOWNLOAD), "Conecto");

                if (DirUtils.create_with_parents (ShareHandler.DOWNLOADS, 0700) == -1) {
                    warning ("Failed to create downloads directory: %s", Posix.strerror (Posix.errno));
                }
            }

            info ("Downloads will be saved to %s", ShareHandler.DOWNLOADS);
            return new ShareHandler ();
        }

        public static string make_downloads_path (string name) {
            return Path.build_filename (ShareHandler.DOWNLOADS, name);
        }

        public static string make_tmp_downloads_path (string name) {
            return Path.build_filename (ShareHandler.DOWNLOADS_TMP, name);
        }

        public string get_pkt_type () {
            return SHARE;
        }

        public void release_device (Device dev) {
            debug ("Release device %s", dev.to_string ());
            dev.message.disconnect (this.message);
        }

        public DownloadTransfer handle_icon (Device dev, Packet pkt, string app_name) {
            string name = app_name;
            debug ("File: %s size: %s", name, format_size (pkt.payload.size));

            var t = new DownloadTransfer (
                dev,
                new InetSocketAddress (dev.host, (uint16) pkt.payload.port),
                pkt.payload.size,
                make_tmp_downloads_path (name));

            try {
                Core.instance ().transfer_manager.push_job (t);
                t.start_async.begin ();
            } catch (Error e) {
                warning ("Error: %s", e.message);
            }
            return t;
        }

        private void message (Device dev, Packet pkt) {
            if (pkt.pkt_type != SHARE_PKT && pkt.pkt_type != SHARE) {
                return;
            }

            if (pkt.body.has_member ("filename")) {
                this.handle_file (dev, pkt);
            } else if (pkt.body.has_member ("url")) {
                this.handle_url (dev, pkt);
            } else if (pkt.body.has_member ("text")) {
                this.handle_text (dev, pkt);
            }
        }

        private void handle_file (Device dev, Packet pkt) {
            if (pkt.payload == null) {
                warning ("Missing payload info");
            return;
            }

            string name = pkt.body.get_string_member ("filename");
            debug ("File: %s size: %s", name, format_size (pkt.payload.size));

            var t = new DownloadTransfer (
                dev,
                new InetSocketAddress (dev.host, (uint16) pkt.payload.port),
                pkt.payload.size,
                make_downloads_path (name)
            );

            try {
                Core.instance ().transfer_manager.push_job (t);
                t.start_async.begin ();
            } catch (Error e) {
                warning ("Error: %s", e.message);
            }
        }

        private void handle_url (Device dev, Packet pkt) {
            var url_msg = pkt.body.get_string_member ("url");
            try {
                var urls = Utils.find_urls (url_msg);
                if (urls.length > 0) {
                    var url = urls[0];
                    debug ("Got URL: %s, launching...", url);
                    AppInfo.launch_default_for_uri (url, null);
                }
            } catch (Error e) {
                warning ("Error: %s", e.message);
            }
        }

        private void handle_text (Device dev, Packet pkt) {
            var text = pkt.body.get_string_member ("text");
            debug ("Shared text '%s'", text);
            var display = Gdk.Display.get_default ();
            if (display != null) {
                var cb = Gtk.Clipboard.get_default (display);
                cb.set_text (text, -1);
            }
        }

        private Packet make_share_packet (string name, string data) {
            var builder = new Json.Builder ();
            builder.begin_object ();
            builder.set_member_name (name);
            builder.add_string_value (data);
            builder.end_object ();
            return new Packet (SHARE, builder.get_root ().get_object ());
        }

        private Packet make_file_share_packet (string filename, uint64 size, uint16 port) {

            var builder = new Json.Builder ();
            builder.begin_object ();
            builder.set_member_name ("filename");
            builder.add_string_value (filename);
            builder.end_object ();

            var pkt = new Packet (SHARE, builder.get_root ().get_object ());
            pkt.payload = Packet.Payload () {
                size = size,
                port = port
            };

            return pkt;
        }

        public void share_url (Device dev, string url) {
            debug ("Share url %s to device %s", url, dev.to_string ());
            dev.send (make_share_packet ("url", url));
        }

        public void share_text (Device dev, string text) {
            debug ("Share text %s to device %s", text, dev.to_string ());
            dev.send (make_share_packet ("text", text));
        }

        public void share_file (Device dev, string path, int file_num, out UploadTransfer out_upload_transfer) throws Error {
            debug ("Share file %s to device %s", path, dev.to_string ());

            var file = File.new_for_uri (path);
            uint64 size = 0;
            try {
                var fi = file.query_info (FileAttribute.STANDARD_SIZE, FileQueryInfoFlags.NONE);
                size = fi.get_size ();
            } catch (Error e) {
                warning ("Failed to obtain file size: %s", e.message);
                return;
            }

            debug ("File size: %llu", size);
            if (size == 0) {
                warning ("Trying to share empty file: %s", path);
                return;
            }

            FileInputStream input;
            try {
                input = file.read ();
            } catch (Error e) {
                warning ("Failed to open source file at path %s:\n%s", file.get_path (), e.message);
                throw e;
            }

            uint16 port;
            var listener = Core.instance ().transfer_manager.make_listener (out port);
            if (listener == null) {
                warning ("Could not allodate a listener.");
                return;
            }

            debug ("Allocated listener on port: %u", port);
            var upload_transfer = new UploadTransfer (dev, listener, input, size, file_num, path);
            out_upload_transfer = upload_transfer;

            Core.instance ().transfer_manager.push_job (upload_transfer);
            upload_transfer.start_async.begin ();

            dev.send (make_file_share_packet (file.get_basename (), size, port));
        }
    }
}

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
 * Onur Sahin (sahinonur2000@hotmail.com)
 */
using Conecto;

namespace MConnect { 
    
    public class ClipboardHandler : GLib.Object, PacketHandlerInterface { 
        
        public const string CLIPBOARD = "kdeconnect.clipboard";
        public const string CLIPBOARD_REQUEST = "kdeconnect.clipboard";

        private Gtk.Clipboard clipboard;
        private ulong callback_id = -1;  // To disconnect from owner_change signal

        private string local_buffer { get; set; default = ""; }
        private string remote_buffer { get; set; default = ""; }

        public string get_pkt_type () {
            return CLIPBOARD;
        }

        construct {
            var display = Gdk.Display.get_default ();
            clipboard = Gtk.Clipboard.get_default (display);
        }
        
        public ClipboardHandler () { 
            debug ("CliboardHandler constructed");
        }

        public static ClipboardHandler instance () {
            return new ClipboardHandler ();
        }

        public void use_device (Device device) {
            debug ("Use device %s for clipboard.", device.to_string ());
            device.message.connect (message);
            callback_id = clipboard.owner_change.connect ((clipboard, event) => {
                clipboard.request_text ((cp, text) => {
                    local_buffer = text;
                    push_clipboard (device);
                });
            });
        }

        public void release_device (Device device) {
            debug ("Release device %s", device.to_string ());
            device.message.disconnect (message);
            clipboard.disconnect (callback_id);
        }

        public void message (Device device, Packet pkt) {
            
            if (pkt.pkt_type != CLIPBOARD) {
                return;
            }

            debug ("Got clipboard packet.");   
    
            string text = pkt.body.get_string_member ("content");
            remote_buffer = text;
            pull_clipboard ();
        }

        private void push_clipboard (Device device) {
            if (local_buffer == null) {
                return;
            }
            if (remote_buffer != local_buffer) {
                remote_buffer = local_buffer;
                var packet = _make_clipboard_packet ();
                device.send (packet);
            }
        }

        private void pull_clipboard () {
            if (local_buffer != remote_buffer) {
                local_buffer = remote_buffer;
                clipboard.set_text (remote_buffer, -1);
            }
        }

        private Packet _make_clipboard_packet () {
            var builder = new Json.Builder ();
            builder.begin_object ();
            builder.set_member_name ("content");
            builder.add_string_value (local_buffer);
            builder.end_object ();

            return new Packet (CLIPBOARD_REQUEST, builder.get_root ().get_object ());
        }
    }
}
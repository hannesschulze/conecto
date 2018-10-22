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

    class PingHandler : Object, PacketHandlerInterface {

        public const string PING = "kdeconnect.ping";
        public const string PING_REQUEST = "kdeconnect.ping";

        public string get_pkt_type () {
            return PING;
        }

        private PingHandler () {
        }

        public static PingHandler instance () {
            return new PingHandler ();
        }

        public void use_device (Device dev) {
            debug ("Use device %s for ping.", dev.to_string ());
            dev.message.connect (this.message);
        }

        public void release_device (Device dev) {
            debug ("Release device %s", dev.to_string ());
            dev.message.disconnect (this.message);
        }

        public void message (Device device, Packet pkt) {

            if (pkt.pkt_type != PING || device.is_capabality_activated (PING) == false) {
                return;
            }

            info ("Got ping packet.");
            debug ("Ping from device %s", device.to_string ());
            string notif_summary = "KDEConnect \u0040" + device.custom_name;
            string notif_body = "<i>Ping received !</i>\n";

            var notification = new GLib.Notification (notif_summary);
                notification.set_body (notif_body);
                notification.set_icon (new GLib.ThemedIcon (Utils.get_icon_name (device.device_type)));
            try {
                Core.instance ().application.send_notification (device.id + "_ping_notif", notification);
            } catch (Error e) {
                warning ("Error: %s", e.message);
            }
        }

        /**
         * send_ping:
         *
         * Ping device.
         */
        public void send_ping (Device device) {
            device.send (this._make_ping_packet ());
        }

        /**
         * _make_ping_packet:
         *
         * @return allocated packet
         */
        private Packet _make_ping_packet () {
            var builder = new Json.Builder ();
            builder.begin_object ();
            builder.set_member_name ("sendPing");
            builder.add_boolean_value (true);
            builder.set_member_name ("messageBody");
            builder.add_string_value ("Ping !");
            builder.end_object ();

            return new Packet (PING_REQUEST, builder.get_root ().get_object ());
        }
    }
}

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
using Gee;
using EOSConnect;
using EOSConnect.Plugin;

namespace MConnect {

    public class Device : Object {

        public const uint PAIR_TIMEOUT = 30;

        public signal void paired (bool pair);
        public signal void connected ();
        public signal void disconnected ();
        public signal void message (Packet pkt);
        /**
         * capability_added:
         * @cap: device capability, eg. kdeconnect.notification
         *
         * Device capability was added
         */
        public signal void capability_added (string cap);
        /**
         * capability_removed:
         * @cap: device capability, eg. kdeconnect.notification
         // *
         * Device capability was removed
         */
        public signal void capability_removed (string cap);

        public GLib.Settings settings { get; private set; }
        public int device_num { get; private set; default = 0; }
        public string id { get; private set; default = ""; }
        public string name { get; private set; default = ""; }
        public string custom_name { get; set; default = ""; }
        public string device_type { get; private set; default = ""; }
        public uint protocol_version { get; private set; default = 7; }
        public uint tcp_port { get; private set; default = 1714; }
        public InetAddress host { get; private set; default = null; }
        public bool is_paired { get; private set; default = false; }
        public bool allowed { get; set; default = false; }
        public bool is_active { get; private set; default = false; }
        public double battery_level { get; set; default = 100; }
        public bool battery_charging { get; set; default = false; }
        public ArrayList<string> outgoing_capabilities { get; private set; default = null; }
        public ArrayList<string> incoming_capabilities { get; private set; default = null; }
        public TlsCertificate certificate = null;
        public string certificate_pem {
            owned get {
                if (certificate == null) {
                    return "";
                }
                return certificate.certificate_pem;
            }
            private set {
            }
        }
        public string certificate_fingerprint { get; private set; default = ""; }
        public Gee.HashMap<string, PluginInterface> plugins_map { get; private set; }

        private HashSet<string> _capabilities = null;
        // Set to true if pair request was sent
        private bool _pair_in_progress = false;
        private uint _pair_timeout_source = 0;
        private DeviceChannel _channel = null;
        // Registered packet handlers
        private HashMap<string, PacketHandlerInterface> _handlers;
        private ArrayList<string> _notification_send_app_banned;

        public Device () {
            try {
                incoming_capabilities = new ArrayList<string> ();
                outgoing_capabilities = new ArrayList<string> ();
                _capabilities = new HashSet<string> ();
                _handlers = new HashMap<string, PacketHandlerInterface> ();
                _notification_send_app_banned = new ArrayList<string> ();
                device_num = Core.instance ().devices_map.size + 1;
            } catch (Error e) {
                warning ("Error: %s", e.message);
            }
        }

        /**
         * Constructs a new Device wrapper based on identity packet.
         *
         * @param pkt identity packet
         * @param host source host that the packet came from
         */
        public Device.from_discovered_device (DiscoveredDevice disc) {
            debug ("[DISCOVERED] new device: %s", disc.name);
            try {
                this();
                host = disc.host;
                name = disc.name;
                custom_name = disc.name;
                id = disc.id;
                device_type = disc.device_type;
                protocol_version = disc.protocol_version;
                tcp_port = disc.tcp_port;
                outgoing_capabilities = new ArrayList<string>.wrap (
                    disc.outgoing_capabilities);
                incoming_capabilities = new ArrayList<string>.wrap (
                    disc.incoming_capabilities);
                setup_plugins ();

                if (Core.instance ().devices_map.has_key (this.to_unique_string ()) == false) {
                    debug ("It's a real new device %s", name);
                    Core.instance ().devices_map.@set (id, this);
                }
            } catch (Error e) {
                warning ("Error: %s", e.message);
            }
        }

        /**
         * Constructs a new Device wrapper based on data read from device
         * cache file.
         *
         * @cache: device cache file
         * @name: device name
         */
        public static Device ? new_from_cache (KeyFile cache, string name) throws Error {
            debug ("[CACHE] new device: %s", name);
            try {
                var dev = new Device ();
                dev.id = cache.get_string (name, "deviceId");
                dev.name = cache.get_string (name, "deviceName");
                dev.custom_name = cache.get_string (name, "deviceName");
                dev.device_type = cache.get_string (name, "deviceType");
                dev.protocol_version = cache.get_integer (name, "protocolVersion");
                dev.tcp_port = (uint) cache.get_integer (name, "tcpPort");
                var last_ip_str = cache.get_string (name, "lastIPAddress");
                debug ("last known address: %s:%u", last_ip_str, dev.tcp_port);
                dev.allowed = cache.get_boolean (name, "allowed");
                dev.is_paired = cache.get_boolean (name, "paired");
                try {
                    var cached_certificate = cache.get_string (name, "certificate");
                    if (cached_certificate != "") {
                        var cert = new TlsCertificate.from_pem (cached_certificate, cached_certificate.length);
                        dev.update_certificate (cert);
                    }
                } catch (KeyFileError e) {
                    if (e is KeyFileError.KEY_NOT_FOUND) {
                        warning ("Device %s using older cache format.", dev.id);
                    } else {
                        throw e;
                    }
                }

                dev.outgoing_capabilities = new ArrayList<string>.wrap (
                    cache.get_string_list (name, "outgoing_capabilities"));
                dev.incoming_capabilities = new ArrayList<string>.wrap (
                    cache.get_string_list (name, "incoming_capabilities"));
                dev.setup_plugins ();
                var host = new InetAddress.from_string (last_ip_str);
                if (host == null) {
                    debug ("Failed to parse last known IP address (%s) for device %s", last_ip_str, name);
                    return null;
                }
                dev.host = host;
                return dev;
            } catch (KeyFileError e) {
                warning ("Failed to load device data from cache: %s", e.message);
                return null;
            }
        }

        ~Device () {
        }

        /**
         * Generates a unique string for this device
         */
        public string to_unique_string () {
            return Utils.make_unique_device_string (id,
                                                    name,
                                                    device_type,
                                                    protocol_version);
        }

        public string to_string () {
            return Utils.make_device_string (id,
                                             name,
                                             device_type,
                                             protocol_version);
        }

        /**
         * Dump device information to cache
         *
         * @cache: device cache
         * @name: group name
         */
        public void to_cache (KeyFile cache, string name) throws Error {
            cache.set_string (name, "deviceId", id);
            cache.set_string (name, "deviceName", name);
            cache.set_string (name, "deviceType", device_type);
            cache.set_integer (name, "protocolVersion", (int) protocol_version);
            cache.set_integer (name, "tcpPort", (int) tcp_port);
            cache.set_string (name, "lastIPAddress", host.to_string ());
            cache.set_boolean (name, "allowed", allowed);
            cache.set_boolean (name, "paired", is_paired);
            cache.set_string (name, "certificate", certificate_pem);
            cache.set_string_list (name, "outgoing_capabilities", outgoing_capabilities.to_array ());
            cache.set_string_list (name, "incoming_capabilities", incoming_capabilities.to_array ());
        }

        /**
         * pair: sent pair request
         *
         * Internally changes pair requests state tracking.
         *
         * @param expect_response se to true if expecting a response
         */
        public async void pair (bool expect_response = true) {
            if (host != null) {
                debug ("Start pairing.");
                if (expect_response == true) {
                    _pair_in_progress = true;
                    _pair_timeout_source = Timeout.add_seconds (PAIR_TIMEOUT, pair_timeout);
                }

                yield _channel.send (Packet.new_pair ());
            }
        }

        /**
         * maybe_pair:
         *
         * Trigger pairing or call handle_pair() if already paired.
         */
        public void maybe_pair () {
            if (is_paired == false) {
                if (_pair_in_progress == false)
                    pair.begin ();
            } else {
                // We are already paired.
                handle_pair (true);
            }
        }

        /**
         * activate:
         *
         * Activate device. Triggers sending of #paired signal after
         * successfuly opening a connection.
         */
        public void activate () {
            if (_channel != null) {
                debug ("Device %s already active", to_string ());
                return;
            }

            _channel = new DeviceChannel (host, tcp_port);
            _channel.disconnected.connect ((c) => {
                handle_disconnect ();
            });
            _channel.packet_received.connect ((c, pkt) => {
                packet_received (pkt);
            });
            _channel.open.begin ((c, res) => {
                channel_opened (_channel.open.end (res));
            });

            is_active = true;
        }

        /**
         * deactivate:
         *
         * Deactivate device.
         */
        public void deactivate () {
            if (_channel != null) {
                close_and_cleanup ();
            }
        }

        /**
         * register_capability_handler:
         * @cap: capability, eg. kdeconnect.notification
         * @h: packet handler
         *
         * Keep track of capability handler @h that supports capability @cap.
         * Register oneself with capability handler.
         */
        public void register_capability_handler (string capability_name, PacketHandlerInterface h) {
            assert (has_capability_handler (capability_name) == false);
            _handlers.@set (capability_name, h);
            // Make handler connect to device.
            h.use_device (this);
        }

        /**
         * has_capability_handler:
         * @cap: capability, eg. kdeconnect.notification
         *
         * Returns true if there is a handler of capability @cap registed for this
         * device.
         */
        public bool has_capability_handler (string capability_name) {
            return _handlers.has_key (capability_name);
        }

        public PacketHandlerInterface? get_path_capability_handler (string capability_name) {
            return _handlers.@get (capability_name);
        }

        /**
         * unregister_capability_handler:
         * @cap: capability, eg. kdeconnect.notification
         *
         * Unregisters a handler for capability @cap.
         */
        public void unregister_capability_handler (string capability_name) {
            PacketHandlerInterface handler;
            _handlers.unset (capability_name, out handler);
            if (handler != null) {
                // Make handler release the device.
                handler.release_device (this);
            }
        }

        /**
         * update_from_device:
         * @other_dev: other device
         *
         * Update information/state of this device using data from @other_dev. This
         * may happen in case when a discovery packet was received, or a device got
         * connected. In such case, a `this` device (which was likely created from
         * cached data) needs to be updated.
         *
         * As a side effect, updating capabilities will emit @capability_added
         * and @capability_removed signals.
         */
        public void update_from_device (Device other_dev) {
            outgoing_capabilities = other_dev.outgoing_capabilities;
            incoming_capabilities = other_dev.incoming_capabilities;

            HashSet<string> added;
            HashSet<string> removed;
            merge_capabilities (out added, out removed);

            foreach (var c in added) {
                debug ("Added: %s", c);
                capability_added (c);
                // update_capabilities_status_available (c, true);
            }

            foreach (var c in removed) {
                debug ("Removed: %s", c);
                capability_removed (c);
                // Remove capability handlers
                unregister_capability_handler (c);
                // update_capabilities_status_available (c, false);
            }

            if (host != null && host.to_string () != other_dev.host.to_string ()) {
                debug ("Host address changed from %s to %s", host.to_string (), other_dev.host.to_string ());
                // Deactivate first
                deactivate ();

                host = other_dev.host;
                tcp_port = other_dev.tcp_port;
            }
        }

        public void send (Packet pkt) {
            // @TODO: queue messages
            if (_channel != null) {
                _channel.send.begin (pkt);
            } else {
                warning ("_channel is null");
            }
        }

        public void setup_plugins () {
            var schema = SettingsSchemaSource.get_default ().lookup (
                EOSConnect.App.GSETTINGS_SCHEMA_ID + ".settings.device",
                false);
            if (schema != null) {
                debug ("Getting config for device ID: " + id);
                settings = new GLib.Settings.full (schema,
                    null,
                    EOSConnect.App.GSETTINGS_SCHEMA_PATH + "/settings/devices/%s/".printf (id));

                if (settings.get_string ("custom-name").length > 1) {
                    custom_name =  settings.get_string ("custom-name");
                }

                plugins_map = new Gee.HashMap<string, PluginInterface> ();
                foreach (string capability_name in incoming_capabilities) {
                    _setup_plugin (capability_name, "incoming");
                }
            } else {
                warning ("Unable to get config for device %s", id);
            }
        }

        public bool is_capabality_activated (string capability_name) {

            if(plugins_map.has_key (capability_name)) {
                return ((PluginInterface)plugins_map.@get (capability_name)).is_active;
            }

            return false;
        }

        public PluginInterface get_plugin (string capability_name) {
            return ((PluginInterface)plugins_map.@get (capability_name));
        }

        private void _setup_plugin (string capability_name, string plugin_type) {
            try {
                string[] capability_name_parts = capability_name.split(".");
                string plugin_capability_name = capability_name_parts[0] + "." + capability_name_parts[1];

                switch (plugin_capability_name) {
                    case BatteryHandler.BATTERY:
                        if(!plugins_map.has_key (BatteryHandler.BATTERY)) {
                            var battery_plugin = new Battery ();
                            battery_plugin.is_active = settings.get_boolean (battery_plugin.settings_key_is_active);
                            battery_plugin.battery_low_level_notify_is_active = settings.get_boolean (
                                "kdeconnect-battery-notify-low-level-active");
                            battery_plugin.battery_low_level_notify_treshold = settings.get_double (
                                "kdeconnect-battery-low-level-treshold");

                            plugins_map.@set (BatteryHandler.BATTERY, battery_plugin);
                        }
                    break;

                    case NotificationHandler.NOTIFICATION:
                        if(!plugins_map.has_key (NotificationHandler.NOTIFICATION)) {

                            var notification_plugin = new Plugin.Notification ();
                            notification_plugin.is_active = settings.get_boolean (
                                notification_plugin.settings_key_is_active);
                            notification_plugin.settings_receive_is_active = settings.get_boolean (
                                "kdeconnect-notifications-receive");
                            notification_plugin.settings_send_is_active = settings.get_boolean (
                                "kdeconnect-notifications-send");
                            notification_plugin.settings_apps_ids_banned = new Gee.ArrayList<string>.wrap (
                                settings.get_strv ("kdeconnect-notifications-send-banned"));

                            plugins_map.@set (NotificationHandler.NOTIFICATION, notification_plugin);
                        }
                    break;

                    case PingHandler.PING:
                        if(!plugins_map.has_key (PingHandler.PING)) {
                            var ping_plugin = new Ping ();
                            ping_plugin.is_active = settings.get_boolean (ping_plugin.settings_key_is_active);

                            plugins_map.set (PingHandler.PING, ping_plugin);
                        }
                    break;

                    case ShareHandler.SHARE_PKT:
                        if(!plugins_map.has_key (ShareHandler.SHARE_PKT)) {
                            var share_plugin = new Share ();
                            share_plugin.is_active = settings.get_boolean (share_plugin.settings_key_is_active);
                            plugins_map.@set (ShareHandler.SHARE_PKT, share_plugin);
                        }
                    break;

                    case TelephonyHandler.TELEPHONY:
                        if(!plugins_map.has_key (TelephonyHandler.TELEPHONY)) {
                            var telephony_plugin = new Telephony ();
                            telephony_plugin.is_active = settings.get_boolean (telephony_plugin.settings_key_is_active);
                            telephony_plugin.application = Core.instance ().application;
                            telephony_plugin.init ();
                            plugins_map.@set (TelephonyHandler.TELEPHONY, telephony_plugin);
                        }
                    break;

                    default:
                        // info ("Unable to find a plugin for this %s capability: %s", plugin_type, capability_name);
                    break;
                }
            } catch (Error e) {
                warning ("Error: %s", e.message);
            }
        }

        private async void greet () throws Error {
            var core = Core.instance ();
            string host_name = Environment.get_host_name ();
            string user = Environment.get_user_name ();
            yield _channel.send (Packet.new_identity (@"$user@$host_name",
                                                      Environment.get_host_name (),
                                                      core.handlers.interfaces,
                                                      core.handlers.interfaces));

            // Switch to secure channel
            var secure = yield _channel.secure (certificate);

            info ("Secure: %s", secure.to_string ());

            if (secure) {
                update_certificate (_channel.peer_certificate);
                maybe_pair ();
            } else {
                warning ("Failed to enable secure channel.");
                close_and_cleanup ();
            }
        }

        private bool pair_timeout () {
            warning ("Pair request timeout.");
            _pair_timeout_source = 0;
            // Handle failed pairing.
            handle_pair (false);
            // Remove timeout source.
            return false;
        }

        /**
         * channel_opened:
         *
         * Callback after DeviceChannel.open() has completed. If the
         * channel was successfuly opened, proceed with handshake.
         */
        private void channel_opened (bool result) {
            debug ("Channel opened: %s", result.to_string ());
            connected ();
            if (result == true) {
                greet.begin ();
            } else {
                // failed to open channel, invoke cleanup
                channel_closed_cleanup ();
            }
        }

        private void packet_received (Packet pkt) {
            if (pkt.pkt_type == Packet.PAIR) {
                handle_pair_packet (pkt);
            } else {
                // We sent a pair request, but got another packet, supposedly meaning we're alredy paired
                // since the device is sending us data.
                if (is_paired == false) {
                    warning ("Not paired and still got a packet, assuming device is paired");
                    handle_pair (true);
                }
                message (pkt);
            }
        }

        /**
         * handle_pair_packet:
         *
         * Handle incoming packet of Packet.PAIR type. Inside, try to
         * guess if we got a response for a pair request, or is this an
         * unsolicited pair request coming from mobile.
         */
        private void handle_pair_packet (Packet pkt) {
            assert (pkt.pkt_type == Packet.PAIR);
            bool pair = pkt.body.get_boolean_member ("pair");
            handle_pair (pair);
        }

        /**
         * handle_pair:
         * @pair: pairing status
         *
         * Update device pair status.
         */
        private void handle_pair (bool pair) {
            if (_pair_timeout_source != 0) {
                Source.remove (_pair_timeout_source);
                _pair_timeout_source = 0;
            }

            debug ("Pair in progress: %s is paired: %s pair: %s",
                   _pair_in_progress.to_string (), is_paired.to_string (),
                   pair.to_string ());
            if (_pair_in_progress == true) {
                // response to host initiated pairing
                if (pair == true) {
                    debug ("Device is paired, pairing complete");
                    is_paired = true;
                } else {
                    warning ("Pairing rejected by device");
                    is_paired = false;
                }
                // Pair completed.
                _pair_in_progress = false;
            } else {
                debug ("Unsolicited pair change from device, pair status: %s", pair.to_string ());
                if (pair == false) {
                    // Unpair from device.
                    is_paired = false;
                } else {
                    // Split brain, pair was not initiated by us, but we were called with information that we are
                    // paired, assume we are paired and send a pair packet, but not expecting a response this time.
                    this.pair.begin (false);
                    is_paired = true;
                }
            }

            paired (is_paired);
        }

        /**
         * handle_disconnect:
         *
         * Handler for DeviceChannel.disconnected() signal
         */
        private void handle_disconnect () {
            debug ("Channel disconnected.");
            close_and_cleanup ();
        }

        private void close_and_cleanup () {
            _channel.close ();
            channel_closed_cleanup ();
        }

        /**
         * channel_closed_cleanup:
         *
         * Single cleanup point after channel has been closed
         */
        private void channel_closed_cleanup () {
            debug ("Close cleanup.");
            _channel = null;
            is_active = false;
            disconnected ();
        }

        /**
         * merge_capabilities:
         * @added[out]: capabilities that were added
         * @removed[out]: capabilities that were removed
         *
         * Merge and update existing `outgoing_capabilities` and
         * `incoming_capabilities`. Returns lists of added and removed capabilities.
         */
        private void merge_capabilities (out HashSet<string> added, out HashSet<string> removed) {

            var caps = new HashSet<string> ();
            caps.add_all (outgoing_capabilities);
            caps.add_all (incoming_capabilities);

            added = new HashSet<string> ();
            added.add_all (caps);

            // @TODO: simplify capability names, eg kdeconnect.telephony.request -> kdeconnect.telephony
            added.remove_all (_capabilities);

            removed = new HashSet<string> ();
            removed.add_all (_capabilities);
            removed.remove_all (caps);

            _capabilities = caps;
        }

        private void update_certificate (TlsCertificate cert) {
            certificate = cert;

            var fingerprint = Crypt.fingerprint_certificate (cert.certificate_pem);
            var sb = new StringBuilder.sized (fingerprint.length * 2 + "sha1:".length);
            sb.append ("sha1:");
            foreach (var b in fingerprint) {
                sb.append_printf ("%02x", b);
            }

            certificate_fingerprint = sb.str;
        }
    }
}

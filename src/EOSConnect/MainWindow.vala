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
using EOSConnect.Plugin;
using EOSConnect.Plugin.Windows;
using EOSConnect.Widgets;
using Gee;
using Granite.Widgets;
using MConnect;

namespace EOSConnect {

    public class MainWindow : Gtk.ApplicationWindow {

        public GLib.Settings main_settings { get; construct; }
        public SMSHistory sms_history_view { get; construct; }
        public Gtk.Stack main_stack { get; construct; }

        private Views.DevicesView devices_view;
        // private Gtk.InfoBar infobar_error;
        // private Gtk.Label info_bar_error_label;
        // private Gtk.Overlay overlay;
        // private Granite.Widgets.OverlayBar overlaybar;

        public MainWindow (Granite.Application application, GLib.Settings main_settings, SMSHistory sms_history_view) {
            Object (
                application: application,
                main_settings: main_settings,
                sms_history_view: sms_history_view
            );

            set_default_size (900, 600);
            set_size_request (750, 500);
        }

        construct {

            var theme_button = new Gtk.Button.from_icon_name ("object-inverse");
            theme_button.tooltip_text = _("Use dark style");
            theme_button.valign = Gtk.Align.CENTER;

            var new_sms_button = new Gtk.Button.from_icon_name ("mail-message-new");
            new_sms_button.tooltip_text = _("Send a new SMS");
            new_sms_button.valign = Gtk.Align.CENTER;

            var headerbar = new Gtk.HeaderBar ();
            headerbar.get_style_context ().add_class ("default-decoration");
            headerbar.show_close_button = true;
            headerbar.pack_start (new_sms_button);
            headerbar.pack_end (theme_button);
            headerbar.title = "EOS Connect";

            set_titlebar (headerbar);
            window_position = Gtk.WindowPosition.CENTER;

            // infobar_error = new Gtk.InfoBar();
            // infobar_error.set_show_close_button(true);
            // infobar_error.message_type = Gtk.MessageType.ERROR;
            // infobar_error.no_show_all = true;
            //
            // info_bar_error_label = new Gtk.Label ("");
            //
            // var info_bar_error_content = infobar_error.get_content_area ();
            // info_bar_error_content.add (info_bar_error_label);

            devices_view = new Views.DevicesView (this);
            var main_settings_view = new Views.MainSettingsView (this, main_settings);

            main_stack = new Gtk.Stack ();
            main_stack.expand = true;
            main_stack.transition_type = Gtk.StackTransitionType.SLIDE_LEFT_RIGHT;
            main_stack.add_titled (devices_view, "devices_view", _("Devices"));
            main_stack.add_titled (sms_history_view, "sms_history_view", _("SMS"));
            main_stack.add_titled (main_settings_view, "main_settings_view", _("Settings"));

            var main_stackswitcher = new Gtk.StackSwitcher ();
            main_stackswitcher.set_stack (main_stack);
            main_stackswitcher.halign = Gtk.Align.CENTER;
            main_stackswitcher.homogeneous = true;

            var refresh_in_progress = false;

            var sub_grid = new Gtk.Grid ();
            sub_grid.orientation = Gtk.Orientation.VERTICAL;
            sub_grid.margin = 0;
            sub_grid.margin_top = 12;
            sub_grid.add (main_stackswitcher);
            sub_grid.add (main_stack);


            var main_grid = new Gtk.Grid ();
            main_grid.orientation = Gtk.Orientation.VERTICAL;
            // main_grid.add(infobar_error);
            main_grid.add(sub_grid);

            // Debug toolbar
            if (Granite.Services.Logger.DisplayLevel == Granite.Services.Logger.DEBUG) {
                var debug_toolbar = new DebugToolbar (devices_view);
                main_grid.add(debug_toolbar);
            }

            // overlay = new Gtk.Overlay ();
            // overlay.add_overlay (main_grid);

            Gtk.Settings.get_default ().gtk_application_prefer_dark_theme = main_settings.get_boolean ("use-dark-theme");

            // add (overlay);
            add (main_grid);

            theme_button.clicked.connect (() => {
                var window_settings = Gtk.Settings.get_default ();
                window_settings.gtk_application_prefer_dark_theme = !window_settings.gtk_application_prefer_dark_theme;
                main_settings.set_boolean ("use-dark-theme", window_settings.gtk_application_prefer_dark_theme);

                if (window_settings.gtk_application_prefer_dark_theme) {
                    theme_button.tooltip_text = _("Use light style");
                } else {
                    theme_button.tooltip_text = _("Use dark style");
                }
            });

            // infobar_error.response.connect ((response_id) => {
            //     infobar_error.no_show_all = true;
            //     infobar_error.hide ();
            // });


            new_sms_button.clicked.connect ( () => { sms_history_view.handle_new_sms_button (new_sms_button); });
        }

        public void update_ui (HashMap<string, Device> devices_map) {
            devices_view.update_device_list (devices_map);
        }

        // public void show_error_infobar (string error_message) {
        //     if (overlaybar != null) { overlaybar.destroy (); } ;
        //     infobar_error.no_show_all = false;
        //     info_bar_error_label.label = "%s".printf (error_message);
        //     infobar_error.show_all ();
        // }
        //
        // public void hide_overlaybar () {
        //     if (overlaybar != null) { overlaybar.destroy (); } ;
        // }
        //
        // public void display_overlaybar (string status) {
        //     overlaybar = new Granite.Widgets.OverlayBar (overlay);
        //     overlaybar.label = _(status);
        //     overlaybar.active = true;
        // }
    }
}

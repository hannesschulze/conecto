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
using EOSConnect;
using EOSConnect.Plugin;
using MConnect;

namespace EOSConnect.Plugin.Widgets {

    public class SMSContactListBoxRow : Gtk.ListBoxRow {

        public weak SMSContact sms_contact { get; construct; }

        private Gtk.Label contact_name_label;
        private Granite.Widgets.Avatar avatar;

        public SMSContactListBoxRow (SMSContact sms_contact) {
            Object (
                sms_contact: sms_contact
            );
        }

        construct {

            var grid = new Gtk.Grid ();
            grid.margin = 6;
            grid.column_spacing = 6;
            avatar = new Granite.Widgets.Avatar ();

            setup_avatar ();

            contact_name_label = new Gtk.Label (sms_contact.custom_name);
            contact_name_label.get_style_context ().add_class (Granite.STYLE_CLASS_H3_LABEL);
            contact_name_label.ellipsize = Pango.EllipsizeMode.END;
            contact_name_label.halign = Gtk.Align.START;
            contact_name_label.valign = Gtk.Align.START;

            var contact_phone_number = new Gtk.Label (sms_contact.phone_number);
            contact_phone_number.margin_top = 2;
            contact_phone_number.use_markup = true;
            contact_phone_number.ellipsize = Pango.EllipsizeMode.END;
            contact_phone_number.halign = Gtk.Align.START;
            contact_phone_number.valign = Gtk.Align.START;

            var box = new Gtk.Box (Gtk.Orientation.HORIZONTAL, 0);
            box.pack_start (contact_phone_number, false, false, 0);

            grid.attach (avatar, 0, 0, 1, 2);
            grid.attach (contact_name_label, 1, 0, 1, 1);
            grid.attach (box, 1, 1, 1, 1);

            add (grid);
        }

        public void update_ui () {
            setup_avatar ();
            contact_name_label.label = sms_contact.custom_name;
        }

        private void setup_avatar () {
            if (sms_contact.contact_image_path.length > 10) {
                try {
                    var size = 32 * get_style_context ().get_scale ();
                    var avatar_pixbuf = new Gdk.Pixbuf.from_file_at_scale (sms_contact.contact_image_path, size, size, false);
                    avatar.pixbuf = avatar_pixbuf;
                } catch (Error e) {
                    warning ("Error: %s", e.message);
                    avatar.show_default (32);
                }
            } else {
                avatar.show_default (32);
            }
        }
    }
}

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
namespace Conecto {

    public class EContacts : ContactsInterface {

        private HashTable<string, E.BookClient> _source_client;
        private SList<E.Contact> _contacts;

        public EContacts () {
            _source_client = new HashTable<string, E.BookClient> (str_hash, str_equal);
            _contacts = new SList<E.Contact> ();
        }

        public override void load_contacts () {
            _open_sources.begin ();
        }

        private async void _open_sources () {
            try {
                var registry = yield new E.SourceRegistry (null);
                //registry.source_added.connect (_add_source);
                registry.list_sources (E.SOURCE_EXTENSION_ADDRESS_BOOK).foreach ((source) => {

                    E.SourceAddressBook address_book = (E.SourceAddressBook)source.get_extension (E.SOURCE_EXTENSION_ADDRESS_BOOK);

                    if(source.enabled == true) {
                        debug ("Discovered source: '%s' with backend: '%s'",
                               source.dup_display_name (),
                               address_book.dup_backend_name ());
                        _add_source (source);
                    }
                });
            } catch (GLib.Error error) {
                critical (error.message);
            }
        }

        private void _add_source (E.Source source) {
            _add_source_async.begin (source);
        }

        private async void _add_source_async (E.Source source) {
            debug ("Adding source: '%s'", source.dup_display_name ());
            try {
                var client = yield E.BookClient.connect (source, 30, null);
                _source_client.insert (source.dup_uid (), client);
            } catch (Error e) {
                error (e.message);
            }

            _load_source (source);

            // Idle.add (() => {
            //     _load_source (source);
            //     return false;
            // });
        }

        private void _load_source (E.Source source) {
            E.BookClient client;
            lock (_source_client) {
                client = _source_client.get (source.dup_uid ());
            }

            if (client == null) {
                return;
            }

            try {
                debug ("Loading contacts from source: '%s'", source.dup_display_name ());

                SList<E.Contact> out_contacts;
                client.get_contacts_sync ("", out out_contacts);

                foreach (E.Contact contact in out_contacts) {
                    string phone_number = "";

                    if (contact.mobile_phone != null) {
                        phone_number = contact.mobile_phone;
                    } else if (contact.primary_phone != null) {
                        phone_number = contact.primary_phone;
                    }

                    if(phone_number.length > 8) {
                        E.PhoneNumber e_phone_number = E.PhoneNumber.from_string (phone_number, null);
                        E.ContactPhoto contact_photo = contact.@get<E.ContactPhoto> (E.ContactField.PHOTO);

                        Contact new_contact = new Contact (
                                         contact.full_name,
                                         e_phone_number.to_string (E.PhoneNumberFormat.INTERNATIONAL).replace (" ", ""),
                                         contact_photo == null ? "" : contact_photo.get_uri ());
                        contacts.append (new_contact);
                        contact_loaded (new_contact);
                    }
                }

            } catch (Error e) {
                error (e.message);
            }
        }
    }
}

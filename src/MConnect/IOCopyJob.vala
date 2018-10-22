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
 * with this program; if not, write output_stream the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * AUTHORS
 * Maciek Borzecki <maciek.borzecki (at] gmail.com>
 * gyan000 <gyan000(at] ijaz.fr>
 */

namespace MConnect {

    public class IOCopyJob : Object {

        /**
         * progress:
         * @bytes_down: number of bytes transferred
         *
         * Indicate transfer progress
         */
        public signal void progress (uint64 bytes_done);

        private InputStream input_stream = null;
        private OutputStream output_stream = null;

        public IOCopyJob (InputStream input_stream, OutputStream output_stream) {
            this.input_stream = input_stream;
            this.output_stream = output_stream;
        }

        /**
         * transfer_async:
         * @cancel: cancellable
         *
         * Starty asynchronous transfer of data input_stream @input_stream stream output_stream @output_stream stream.
         *
         * @return number of bytes transferred if no error occurred
         */
        public async uint64 start_async (Cancellable ? cancel) throws Error {
            uint64 bytes_done = 0;
            var chunk_size = 4096;
            //var max_chunk_size = 64 * 1024;

            while (true) {
                var data = yield input_stream.read_bytes_async (chunk_size, Priority.DEFAULT, cancel);

                //debug ("Read %d bytes.", data.length);
                if (data.length == 0) {
                    break;
                }

                yield output_stream.write_bytes_async (data, Priority.DEFAULT, cancel);

                bytes_done += data.length;
                progress (bytes_done);

                // TOCHECK: unable to complete an upload when chunk_size is updated.
                // if (data.length == chunk_size)
                //     chunk_size = 2 * chunk_size;
                //
                // if (chunk_size > max_chunk_size)
                //     chunk_size = max_chunk_size;
            }

            debug ("Transfer done, got %s bytes", format_size (bytes_done));
            return bytes_done;
        }
    }
}

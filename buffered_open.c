#include "buffered_open.h"
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

buffered_file_t *buffered_open(const char *pathname, int flags, ...) {

   // Extract O_PREAPPEND flag
    int preappend = (flags & O_PREAPPEND) ? 1 : 0;
    flags &= ~O_PREAPPEND;  // Remove O_PREAPPEND from flags

    // Allocate memory for buffered_file_t
    buffered_file_t *bf = (buffered_file_t *)malloc(sizeof(buffered_file_t));
    if (!bf) {
        perror("Memory allocation error");
        return NULL;
    }

    // Initialize fields
    bf->fd = -1;  // Set to -1 initially
    bf->read_buffer = (char *)malloc(BUFFER_SIZE);
    bf->write_buffer = (char *)malloc(BUFFER_SIZE);
    bf->read_buffer_size = BUFFER_SIZE;
    bf->write_buffer_size = BUFFER_SIZE;
    bf->read_buffer_pos = 0;
    bf->write_buffer_pos = 0;
    bf->flags = flags;
    bf->preappend = preappend;

    // Open the file
    va_list args;
    va_start(args, flags);
    mode_t mode = va_arg(args, mode_t);
    va_end(args);
    
    bf->fd = open(pathname, flags, mode);
    if (bf->fd == -1) {
        perror("Error opening file");
        free(bf->read_buffer);
        free(bf->write_buffer);
        free(bf);
        return NULL;
    }


    return bf;
}



ssize_t buffered_write(buffered_file_t *bf, const void *buf, size_t count) {

    const char *buf_ptr = (const char *)buf;
    size_t bytes_written = 0;


 if (bf->preappend) {

        // Read existing file content into a temporary buffer or use write buffer if not empty
        char *temp_buffer = NULL;
        ssize_t existing_content_size = 0;


        if (bf->write_buffer_pos > 0) {



            // Use the existing content in write buffer as temporary buffer
            temp_buffer = (char *)malloc(bf->write_buffer_pos);
            if (!temp_buffer) {
                perror("Memory allocation error");
                return -1;
            }

            memcpy(temp_buffer, bf->write_buffer, bf->write_buffer_pos);
            existing_content_size = bf->write_buffer_pos;

             // Reset write buffer position to start fresh
             bf->write_buffer_pos = 0;
        } else {
            // Read existing file content into temporary buffer
            off_t current_offset = lseek(bf->fd, 0, SEEK_CUR); // Get current file offset
            off_t file_size = lseek(bf->fd, 0, SEEK_END);     // Get file size
            lseek(bf->fd, current_offset, SEEK_SET);          // Restore file offset


            if (file_size > 0) {
                temp_buffer = (char *)malloc(file_size);
                if (!temp_buffer) {
                    perror("Memory allocation error");
                    return -1;
                }

                existing_content_size = pread(bf->fd, temp_buffer, file_size, 0);
                if (existing_content_size == -1) {
                    perror("Error reading existing file content");
                    free(temp_buffer);
                    return -1;
                }

            }
        }

        // Copy new data to the write buffer
        while (bytes_written < count) {
            size_t space_left = bf->write_buffer_size - bf->write_buffer_pos;
            size_t remaining_bytes = count - bytes_written;
            size_t bytes_to_write = (remaining_bytes < space_left) ? remaining_bytes : space_left;

            memcpy(bf->write_buffer + bf->write_buffer_pos, buf_ptr + bytes_written, bytes_to_write);
            bf->write_buffer_pos += bytes_to_write;
            bytes_written += bytes_to_write;

            if (bf->write_buffer_pos == bf->write_buffer_size) {
                if (buffered_flush(bf) == -1) {
                    free(temp_buffer);
                    return -1;
                }
            }
        }

        // Append existing content from temp_buffer to the write buffer
        size_t temp_buffer_pos = 0;
        while (temp_buffer_pos < existing_content_size) {
            size_t space_left = bf->write_buffer_size - bf->write_buffer_pos;
            size_t remaining_bytes = existing_content_size - temp_buffer_pos;
            size_t bytes_to_write = (remaining_bytes < space_left) ? remaining_bytes : space_left;

            memcpy(bf->write_buffer + bf->write_buffer_pos, temp_buffer + temp_buffer_pos, bytes_to_write);
            bf->write_buffer_pos += bytes_to_write;
            temp_buffer_pos += bytes_to_write;


            if (bf->write_buffer_pos == bf->write_buffer_size) {
                if (buffered_flush(bf) == -1) {
                    free(temp_buffer);
                    return -1;
                }
            }
        }

        // Free temporary buffer if allocated
        if (temp_buffer) {
            free(temp_buffer);
        }


        return count;

    } else {

            //regular (part 1)
        while (bytes_written < count) {

            //the remaining space in the write buffer 
            size_t space_left = bf->write_buffer_size - bf->write_buffer_pos;
            //Determines how many bytes can currently be written to the buffer 
            size_t remaining_bytes = count - bytes_written;
            size_t bytes_to_write = (remaining_bytes < space_left) ? count - bytes_written : space_left; 

            // Copy data to write buffer
            memcpy(bf->write_buffer + bf->write_buffer_pos, buf_ptr + bytes_written, bytes_to_write);
            bf->write_buffer_pos += bytes_to_write;
            bytes_written += bytes_to_write;

            // Flush if write buffer is full (no space left)
            space_left = bf->write_buffer_size - bf->write_buffer_pos;


            if (space_left == 0) {
                if (buffered_flush(bf) == -1) {
                    return -1;
                }
            }

        }

        return bytes_written;
    }
}

ssize_t buffered_read(buffered_file_t *bf, void *buf, size_t count) {
    
    char *buf_ptr = (char *)buf;
    size_t bytes_read = 0;


    if (bf->preappend) {
        // Flush write buffer before reading
        if (buffered_flush(bf) == -1) {
            return -1;
        }
    }

      // Check if read buffer needs to be filled initially
    if (bf->read_buffer_pos == 0) {
        ssize_t bytes_read_now = read(bf->fd, bf->read_buffer, bf->read_buffer_size);
        if (bytes_read_now == -1) {
            perror("Error reading from file");
            return -1;
        } else if (bytes_read_now == 0) {
            // End of file
            return 0;
        }
    }

         
    while (bytes_read < count) {

        // Check if read buffer has data we didn't read yet 
        if (bf->read_buffer_pos < bf->read_buffer_size) {

            size_t space_left = bf->read_buffer_size - bf->read_buffer_pos; //in the read buffer
            size_t remaining_bytes = count - bytes_read; //how many left to read 
            size_t bytes_to_read = (remaining_bytes < space_left) ? remaining_bytes : space_left;

            // Copy data from read buffer
            memcpy(buf_ptr + bytes_read, bf->read_buffer + bf->read_buffer_pos, bytes_to_read);
            bf->read_buffer_pos += bytes_to_read;
            bytes_read += bytes_to_read;


        } else { // we read all the data in the read buffer
            // Read from file into read buffer
            ssize_t bytes_read_now = read(bf->fd, bf->read_buffer, bf->read_buffer_size);
            if (bytes_read_now == -1) {
                perror("Error reading from file");
                return -1;
            } else if (bytes_read_now == 0) {
                // End of file
                break;
            }
            bf->read_buffer_pos = 0;  // Reset buffer position

        }
    }
    return bytes_read;

}


int buffered_flush(buffered_file_t *bf) {

    ssize_t bytes_written = write(bf->fd, bf->write_buffer, bf->write_buffer_pos);
    if (bytes_written == -1) {
        perror("Error writing to file");
        return -1;
    }

    bf->write_buffer_pos = 0;  // Reset write buffer position
    return 0;
}


int buffered_close(buffered_file_t *bf) {
    if (buffered_flush(bf) == -1) {
        return -1;
    }

    int result = close(bf->fd);
    free(bf->read_buffer);
    free(bf->write_buffer);
    free(bf);
    return result;
}

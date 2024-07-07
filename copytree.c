#include "copytree.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <linux/limits.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>




void copy_file(const char *src, const char *dest, int copy_symlinks, int copy_permissions) {

    struct stat src_stat, dest_stat;

     // Get information about the source file
    if (lstat(src, &src_stat) == -1) {
        perror("COMMAND failed");
        return;
    }

     // Check if the destination file already exists
    if (stat(dest, &dest_stat) == 0) {
        perror("COMMAND failed");
        exit(EXIT_FAILURE);

    }

    // Open source file for reading
    int src_fd = open(src, O_RDONLY);
    if (src_fd == -1) {
        perror("COMMAND failed");
        return;
    }


    // Check if the source is a symbolic link
    if (S_ISLNK(src_stat.st_mode)) {

        if (copy_symlinks) {
                // If copying symbolic links as links, create a symbolic link
                if (symlink(src, dest) == -1) {
                    perror("COMMAND failed");

                    close(src_fd);
                    return;
                }

         } else {

      // If not copying as symbolic links, read the target file content and copy it
            int target_fd = open(src, O_RDONLY);
            if (target_fd == -1) {
              perror("COMMAND failed");
                return;
            }

            int dest_fd = open(dest, O_WRONLY | O_CREAT | O_TRUNC, 0666);
            if (dest_fd == -1) {
               perror("COMMAND failed");
                close(target_fd);
                return;
            }

            // Copy file contents
            char buffer[4096];
            ssize_t bytes_read, bytes_written;

            while ((bytes_read = read(target_fd, buffer, sizeof(buffer))) > 0) {
             
                bytes_written = write(dest_fd, buffer, bytes_read);
                if (bytes_written != bytes_read) {
                       perror("COMMAND failed");

                    close(target_fd);
                    close(dest_fd);
                    return;
                }
            }

            if (bytes_read == -1) {
                  perror("COMMAND failed");

                close(target_fd);
                close(dest_fd);
                return;
            }

            close(target_fd);
            close(dest_fd);

                // Set permissions of destination file if copy_permissions is enabled
            if (copy_permissions) {

            // Get source file permissions
            struct stat src_stat;
            if (fstat(src_fd, &src_stat) == -1) {
                perror("COMMAND failed");
                close(src_fd);
                return;
            }

            // Close source file
            close(src_fd);

            // Set permissions of destination file
            if (chmod(dest, src_stat.st_mode) == -1) {
                perror("COMMAND failed");
                return;
            }
        }


        }
    } else { //regular file 
  
        // Open destination file for writing
         int dest_fd;
        // Otherwise, open destination file for writing (copy content)
        dest_fd = open(dest, O_WRONLY | O_CREAT , 0666);
        if (dest_fd == -1) {
            perror("COMMAND failed");
            close(src_fd);
            return;
        }

        // Copy file contents
        char buffer[4096];
        ssize_t bytes_read, bytes_written;
        while ((bytes_read = read(src_fd, buffer, sizeof(buffer))) > 0) {
            bytes_written = write(dest_fd, buffer, bytes_read);
            if (bytes_written != bytes_read) {
                 perror("COMMAND failed");

                close(src_fd);
                close(dest_fd);
                return;
            }
        }
        if (bytes_read == -1) {
            perror("COMMAND failed");
            close(src_fd);
            close(dest_fd);
            return;
        }
        // Close destination file
        close(dest_fd);
    
        // Set permissions of destination file if copy_permissions is enabled
        if (copy_permissions) {

        // Get source file permissions
        struct stat src_stat;
        if (fstat(src_fd, &src_stat) == -1) {
            perror("COMMAND failed");
            close(src_fd);
            return;
        }

         // Close source file
        close(src_fd);

        // Set permissions of destination file
        if (chmod(dest, src_stat.st_mode) == -1) {
            perror("COMMAND failed");
            return;
        }
      }
    }
}




// Helper function to create directories recursively
int mkdir_p(const char *path, mode_t mode) {
    char temp[PATH_MAX];
    char *p = NULL;
    size_t len;

    // Copy the path to the buffer
    snprintf(temp, sizeof(temp), "%s", path);
    len = strlen(temp);

    // Remove trailing slash if it exists
    if (temp[len - 1] == '/') {
        temp[len - 1] = 0;
    }

    // Iterate through the path, creating directories
    for (p = temp + 1; *p; p++) {
        if (*p == '/') {
            *p = 0;
            // Attempt to create the directory
            if (mkdir(temp, 0775) != 0 && errno != EEXIST) {
                return -1;   // Return -1 on failure (except if directory already exists)
            }
            *p = '/'; // Restore the '/' and continue
        }
    }


    // Create the final directory
    if (mkdir(temp, 0755) != 0 && errno != EEXIST) {
        return -1;   // Return -1 on failure (except if directory already exists)
    }


     // Set permissions explicitly after creation
    if (chmod(temp, mode) == -1) {
            perror("COMMAND failed");
        return -1;
    }

      struct stat st;
    if (lstat(temp, &st) != 0) {
         perror("COMMAND failed");
    }

    return 0;   // Return 0 on success
}


void copy_directory(const char *src, const char *dest, int copy_symlinks, int copy_permissions) {
    DIR *dir = opendir(src);
    if (!dir) {
        perror("COMMAND failed");
        return;
    }


    struct stat src_stat;
    if (lstat(src, &src_stat) == -1) {
        perror("COMMAND failed");
        closedir(dir);
        return;
    }


    // Use 0755 if copy_permissions is not enabled, else use the source directory's permissions
    mode_t mode = copy_permissions ? src_stat.st_mode : 0755;

    
      

    // Create the destination directory if it doesn't exist
    if (mkdir_p(dest, mode) != 0) {
        perror("COMMAND failed");
        closedir(dir);
        return;
    }

    struct dirent *entry;
    struct stat statbuf;
    char src_path[PATH_MAX];
    char dest_path[PATH_MAX];

    while ((entry = readdir(dir)) != NULL) {
        // Skip special entries "." and ".."
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // Construct full source and destination paths
        snprintf(src_path, sizeof(src_path), "%s/%s", src, entry->d_name);
        snprintf(dest_path, sizeof(dest_path), "%s/%s", dest, entry->d_name);

        // Get information about the source entry
        if (lstat(src_path, &statbuf) == -1) {
            perror("COMMAND failed");
            continue;
        }

      // Handle symbolic links to directories
        if (S_ISLNK(statbuf.st_mode)) {
            // Symbolic link found
            if (copy_symlinks) {
                // Create a symbolic link in the destination
                char link_target[PATH_MAX];
                ssize_t len = readlink(src_path, link_target, sizeof(link_target) - 1);
                if (len == -1) {
                     perror("COMMAND failed");
                    continue;
                }
                link_target[len] = '\0';
                // printf("Creating symbolic link '%s' to target '%s'\n", dest_path, link_target); // Print symbolic link and target path
                if (symlink(link_target, dest_path) == -1) {
                      perror("COMMAND failed");

                    continue;
                }
            } else {
                 // Read the target of the symbolic link
        char link_target[PATH_MAX];
        ssize_t len = readlink(src_path, link_target, sizeof(link_target) - 1);
        if (len == -1) {
             perror("COMMAND failed");

            continue;
        }
        link_target[len] = '\0';

        // Get information about the target of the symbolic link
        struct stat target_stat;
        if (stat(link_target, &target_stat) == -1) {
             perror("COMMAND failed");

            continue;
        }

        // Check if the target is a directory
        if (S_ISDIR(target_stat.st_mode)) {
            // Copy the contents inside the target directory
            copy_directory(link_target, dest_path, copy_symlinks, copy_permissions);
        } else {
            // Copy the target file
            copy_file(link_target, dest_path, copy_symlinks, copy_permissions);
        }
    
            }
        } else if (S_ISDIR(statbuf.st_mode)) {
          
            copy_directory(src_path, dest_path, copy_symlinks, copy_permissions);
        } else {
         
            copy_file(src_path, dest_path, copy_symlinks, copy_permissions);
        }
    }

    closedir(dir);
}
#include "copytree.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <bits/getopt_core.h>
#include <linux/limits.h>   


char *get_absolute_path(const char *relative_path) {
    char *absolute_path = malloc(PATH_MAX + 1);
    if (absolute_path == NULL) {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }
    
    if (realpath(relative_path, absolute_path) == NULL) {
        perror("realpath failed");
        free(absolute_path);
        return NULL;
    }
    
    return absolute_path;
}


void print_usage(const char *prog_name) {
    fprintf(stderr, "Usage: %s [-l] [-p] <source_directory> <destination_directory>\n", prog_name);
    fprintf(stderr, "  -l: Copy symbolic links as links\n");
    fprintf(stderr, "  -p: Copy file permissions\n");
}

int main(int argc, char *argv[]) {
    int opt;
    int copy_symlinks = 0;
    int copy_permissions = 0;

    // Handle the flags using getopt
    while ((opt = getopt(argc, argv, "lp")) != -1) {
        switch (opt) {
            case 'l':
                copy_symlinks = 1;
                break;
            case 'p':
                copy_permissions = 1;
                break;
            default:
                print_usage(argv[0]);
                return EXIT_FAILURE;
        }
    }


    // Ensure we have exactly two positional arguments remaining (source and destination directories)
    if (optind + 2 != argc) {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    const char *src_dir = argv[optind];
    const char *dest_dir = argv[optind + 1];
    


    char *source_abs_path = get_absolute_path(src_dir);

    
    // Call the function to copy the directory
    copy_directory(source_abs_path, dest_dir, copy_symlinks, copy_permissions);
    

    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>


void write_message(const char *message, int count) {
    for (int i = 0; i < count; i++) {
        printf("%s", message);
        printf("\n");
        usleep((rand() % 100) * 1000); // Random delay between 0 and 99 milliseconds
    }
}


void acquire_lock(const char *lockfile) {
         while (open(lockfile, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH) == -1) {
        usleep((rand() % 100) * 1000); // Sleep for a random time between retries
    }

}

void release_lock(const char *lockfile) {
    unlink(lockfile);
}


int main(int argc, char *argv[]) {

if (argc < 5) {
    fprintf(stderr,"Usage: %s <message1> <message2> ... <count>", argv[0]);
    return 1;
}

    int num_messages = atoi(argv[argc - 1]);

    const char *lockfile = "lockfile.lock";
    const char *outputfile = "output2.txt";

    int fd = open(outputfile,  O_CREAT| O_WRONLY , S_IRWXU | S_IRWXG | S_IRWXO);
    if (fd == -1) {
    perror(NULL);
    return 1;
    }
    close(fd);

    for (int i = 1; i < argc - 1; i++) {
        pid_t pid = fork();
        if (pid < 0) {
            perror("Failed to fork"); //check if we can print anythinggggg
            return 1;
        } else if (pid == 0) { // Child process

        //entry (lock)
        acquire_lock(lockfile);

         fd = open(outputfile, O_WRONLY | O_APPEND);
            if (fd == -1) {
                perror("Failed to open output file");
                release_lock(lockfile);
                exit(1);
            }

            //Redirect stdout to the output file
            dup2(fd, STDOUT_FILENO);
            close(fd);

             // Write the message the specified number of times
            write_message(argv[i], num_messages);

        //exit (unlock)
           release_lock(lockfile);
            exit(0);

        }
    }

     // Parent process waits for all children to finish
    for (int i = 1; i < argc - 1; i++) {
        wait(NULL);
    }

    return 0;


}
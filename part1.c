#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <string.h>


// Function to trim newline characters from a string
void trim_newline(char *str) {
    int len = strlen(str);
    while (len > 0 && (str[len - 1] == '\n' || str[len - 1] == '\r')) {
        str[len - 1] = '\0'; // Replace newline with null terminator
        len--;
    }
}


void write_to_file(const char *filename, const char *message, int count) {
    int fd = open(filename, O_WRONLY | O_APPEND);
     if (fd == -1) {
        perror(NULL);
        exit(1);
    }


      for (int i = 0; i < count; i++) {
        if (write(fd, message, strlen(message)) == -1) {
            perror(NULL);
            close(fd);
            exit(1);
        }
        if (write(fd, "\n", 1) == -1) { // Add a newline after each message
            perror(NULL);
            close(fd);
            exit(1);
        }
    }

    close(fd);
}

int main(int argc, char *argv[]) {

    if (argc != 5) {
        fprintf(stderr, "Usage: %s <parent_message> <child1_message> <child2_message> <count>\n", argv[0]);
        return 1;
    }

        // Trim newline characters from each message argument
    trim_newline(argv[1]); // Parent message
    trim_newline(argv[2]); // Child1 message
    trim_newline(argv[3]); // Child2 message

    char *parent_message = argv[1];
    char *child1_message = argv[2];
    char *child2_message = argv[3];
    int count = atoi(argv[4]);


    int fd = open("output.txt",  O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);
    if (fd == -1) {
    perror(NULL);
    return 1;
    }
    close(fd);

    pid_t pid1, pid2;
    // Fork the first child
    pid1 = fork();
    if (pid1 < 0) {
        perror(NULL);
        return 1;
    }

    if (pid1 == 0) {

        write_to_file("output.txt", child1_message, count);
        return 0;
    }

    // Fork the second child
    pid2 = fork();
    if (pid2 < 0) {
        perror(NULL);
        return 1;
    }

    if (pid2 == 0) {
        // Child 2 process
        sleep(3);

        write_to_file("output.txt", child2_message, count);
        return 0;
    }

    // Parent process waits for both children to finish
    wait(NULL);
    wait(NULL);

    // Parent process writes
    write_to_file("output.txt", parent_message, count);

    return 0;



}



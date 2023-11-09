#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main() {
    int pipe1[2];
    int pipe2[2];
    pid_t pid1, pid2;
    char message[100];

    if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
        perror("Pipe creation failed");
        exit(1);
    }

    // Create the first child process
    if ((pid1 = fork()) == -1) {
        perror("Fork failed");
        exit(1);
    }

    if (pid1 == 0) { // First child process
        close(pipe1[1]); // Close the write end of the first pipe
        close(pipe2[0]); // Close the read end of the second pipe

        // Read the message from the parent
        read(pipe1[0], message, sizeof(message));
        printf("Child 1 received message: %s\n", message);

        // Modify the message (e.g., make it uppercase)
        for (int i = 0; i < strlen(message); i++) {
            message[i] = toupper(message[i]);
        }

        // Send the modified message to the second child
        write(pipe2[1], message, strlen(message) + 1);

        close(pipe1[0]);
        close(pipe2[1]);
        exit(0);
    } else { // Parent process
        // Create the second child process
        if ((pid2 = fork()) == -1) {
            perror("Fork failed");
            exit(1);
        }

        if (pid2 == 0) { // Second child process
            close(pipe2[1]); // Close the write end of the second pipe

            // Read the modified message from the first child
            read(pipe2[0], message, sizeof(message));
            printf("Child 2 received message: %s\n", message);

            close(pipe2[0]);
            exit(0);
        } else { // Parent process
            close(pipe1[0]); // Close the read end of the first pipe
            close(pipe2[0]); // Close the read end of the second pipe

            // Send a message to the first child
            strcpy(message, "Hello, child 1!");
            write(pipe1[1], message, strlen(message) + 1);

            close(pipe1[1]);
            close(pipe2[1]);

            // Wait for both child processes to finish
            wait(NULL);
            wait(NULL);
        }
    }

    return 0;
}

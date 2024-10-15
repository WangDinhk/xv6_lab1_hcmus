#include "kernel/types.h"
#include "user/user.h"

int main() {
    char data = 'a';
    int dataSize = sizeof(data);

    // Create two pipes to communicate between parent and child
    // Index 0 is for reading, index 1 is for writing
    int parentToChildPipe[2];
    int childToParentPipe[2];

    // Create the pipes
    // pipe(parentToChildPipe) 
    // pipe(childToParentPipe)
    if (pipe(parentToChildPipe) == -1 || pipe(childToParentPipe) == -1) {
        printf("Error: fail to create the pipe\n");
        exit(1);
    }

    // Fork the process
    int processID = fork();

    if (processID < 0) {
        printf("Error: fail to fork\n");
        exit(1);
    }
    else if (processID != 0) { // Parent process
        // Close the read from parent to child pipe
        close(parentToChildPipe[0]);
        // Close the write from child to parent pipe
        close(childToParentPipe[1]);

        // Write the data from parent to the child
        if (write(parentToChildPipe[1], &data, dataSize) != dataSize) {
            printf("Error: fail to write to the pipe\n");
            exit(1);
        }
        // Read the data from the child
        if (read(childToParentPipe[0], &data, dataSize) != dataSize) {
            printf("Error: fail to read from the pipe\n");
            exit(1);
        }

        printf("%d: received pong\n", getpid());

        // Close the pipes
        close(parentToChildPipe[1]);
        close(childToParentPipe[0]);

        // Parent wait for the child to finish to avoid zombie process 
        wait(0);
    }
    else { // Child process (processID == 0)
        // Close the write from parent to child pipe
        close(parentToChildPipe[1]);
        // Close the read from child to parent pipe
        close(childToParentPipe[0]);

        // Read the data from the parent
        if (read(parentToChildPipe[0], &data, dataSize) != dataSize) {
            printf("Error: fail to read from the pipe\n");
            exit(1);
        }

        printf("%d: received ping\n", getpid());

        // Write the data back to the parent
        if (write(childToParentPipe[1], &data, dataSize) != dataSize) {
            printf("Error: fail to write to the pipe\n");
            exit(1);
        }
        
        // Close the pipes
        close(parentToChildPipe[0]);
        close(childToParentPipe[1]);
    }

    exit(0);
}
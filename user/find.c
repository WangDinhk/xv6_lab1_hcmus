#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "user/user.h"

void find(const char* directoryPath, const char *targetFilename) {
    char fullPathBuffer[512]; 
    char* pathPointer;
    int fileDescriptor;

    struct dirent directoryEntry; // Structure of directory entry
    struct stat fileStatus; // Structure of file status

    // Open the directory
    if ((fileDescriptor = open(directoryPath, 0)) < 0) {
        fprintf(2, "Error: cannot open %s\n", directoryPath);
        return;
    }

    // Get the file status
    if (fstat(fileDescriptor, &fileStatus) < 0) {
        fprintf(2, "Error: cannot stat %s\n", directoryPath);
        close(fileDescriptor);
        return;
    }

    // Check if the file is a directory
    if (fileStatus.type != T_DIR) {
        fprintf(2, "Error: %s is not a directory\n", directoryPath);
        close(fileDescriptor);
        return;
    }

    // Copy the directory path to the full path buffer
    if ((strlen(directoryPath) + 1 + DIRSIZ + 1) > sizeof(fullPathBuffer)) {
        fprintf(2, "Error: path is too long\n");
        close(fileDescriptor);
        return;
    }
    strcpy(fullPathBuffer, directoryPath);
    pathPointer = fullPathBuffer + strlen(fullPathBuffer);
    // Add the trailing slash
    *pathPointer++ = '/';

    // Read the directory entries
    while (read(fileDescriptor, &directoryEntry, sizeof(directoryEntry)) == sizeof(directoryEntry)) {
        // Skip the empty directory
        if (directoryEntry.inum == 0) {
            continue;
        }

        // Skip "." and ".." directory
        if (strcmp(directoryEntry.name, ".") == 0 || strcmp(directoryEntry.name, "..") == 0) {
            continue;
        }

        // Copy the directory entry name to the full path buffer
        memmove(pathPointer, directoryEntry.name, DIRSIZ);
        pathPointer[strlen(directoryEntry.name)] = '\0';

        // Get the file status
        if (stat(fullPathBuffer, &fileStatus) < 0) {
            fprintf(2, "Error: cannot stat %s\n", fullPathBuffer);
            continue;
        }

        // Check if the file is a file
        // and the file name matches the target file name
        if (fileStatus.type == T_FILE && strcmp(directoryEntry.name, targetFilename) == 0) {
            printf("%s\n", fullPathBuffer);
        }
        // Check if the file is a directory
        else if (fileStatus.type == T_DIR) {
            // Recursively search the directory
            find(fullPathBuffer, targetFilename);
        }
    }

    close(fileDescriptor);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(2, "Usage: find <path> <filename>\n");
        exit(1);
    }

    find(argv[1], argv[2]);

    exit(0);
}
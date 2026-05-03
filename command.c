#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include "command.h"
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "string_parser.h"

void listDir() {
    struct dirent *entry;
    DIR *dp = opendir(".");

    while ((entry = readdir(dp))) {
        char* list = entry->d_name;
        fputs(list, stdout);
        fputs(" ", stdout);
    }
    puts("");
    closedir(dp);
}

void showCurrentDir() {
    char pwd[1024];
    if (getcwd(pwd, sizeof(pwd)) != NULL) {
        puts(pwd);
    } else {
        exit(EXIT_FAILURE);
    }
}

void makeDir(char *dirName) {
    struct stat st;
    if (stat(dirName, &st) == 0) {
        puts("Directory already exists!");
        return;
    }
    mkdir(dirName, 0755);
}

void changeDir(char *dirName) {
    if (chdir(dirName) != 0) {
        puts("Directory does not exist!");
    }
}

void copyFile(char *sourcePath, char *destinationPath) {
    char buffer[1024];
    char *destinationTarget = NULL;
    struct stat dstStat;
    int source = open(sourcePath, O_RDONLY);
    if (source == -1) {
        puts("Source file does not exist!");
        return;
    }

    if (stat(destinationPath, &dstStat) == 0 && S_ISDIR(dstStat.st_mode)) {
        const char *base = strrchr(sourcePath, '/');
        size_t targetLen;
        base = (base == NULL) ? sourcePath : base + 1;
        targetLen = strlen(destinationPath) + 1 + strlen(base) + 1;
        destinationTarget = (char *)malloc(targetLen);
        if (destinationTarget == NULL) {
            close(source);
            return;
        }
        strcpy(destinationTarget, destinationPath);
        strcat(destinationTarget, "/");
        strcat(destinationTarget, base);
    } else {
        destinationTarget = (char *)malloc(strlen(destinationPath) + 1);
        if (destinationTarget == NULL) {
            close(source);
            return;
        }
        strcpy(destinationTarget, destinationPath);
    }

    int destination = open(destinationTarget, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (destination == -1) {
        puts("Could not open destination file!");
        close(source);
        free(destinationTarget);
        return;
    }

    ssize_t n = 0;
    while ((n = read(source, buffer, sizeof(buffer))) > 0) {
        ssize_t written = 0;
        while (written < n) {
            ssize_t w = write(destination, buffer + written, (size_t)(n - written));
            if (w == -1) {
                puts("Failed while writing destination file!");
                close(source);
                close(destination);
                free(destinationTarget);
                return;
            }
            written += w;
        }
    }

    if (n == -1) {
        puts("Failed while reading source file!");
    }

    close(source);
    close(destination);
    free(destinationTarget);
}

void moveFile(char *sourcePath, char *destinationPath) {
    char *destinationTarget = NULL;
    struct stat srcStat;
    struct stat dstStat;

    if (stat(sourcePath, &srcStat) != 0 || S_ISDIR(srcStat.st_mode)) {
        puts("Source file does not exist!");
        return;
    }

    if (stat(destinationPath, &dstStat) == 0 && S_ISDIR(dstStat.st_mode)) {
        const char *base = strrchr(sourcePath, '/');
        size_t targetLen;
        base = (base == NULL) ? sourcePath : base + 1;
        targetLen = strlen(destinationPath) + 1 + strlen(base) + 1;
        destinationTarget = (char *)malloc(targetLen);
        if (destinationTarget == NULL) {
            return;
        }
        strcpy(destinationTarget, destinationPath);
        strcat(destinationTarget, "/");
        strcat(destinationTarget, base);
    } else {
        destinationTarget = (char *)malloc(strlen(destinationPath) + 1);
        if (destinationTarget == NULL) {
            return;
        }
        strcpy(destinationTarget, destinationPath);
    }

    if (rename(sourcePath, destinationTarget) != 0) {
        puts("Failed to move file!");
    }

    free(destinationTarget);
}

void deleteFile(char *filename) {
    struct stat st;

    // Check if the file exists before attempting to delete
    if (access(filename, F_OK) != 0) {
        puts("File does not exist!");
        return;
    }

    if (stat(filename, &st) != 0) {
        puts("File does not exist!");
        return;
    }

    if (S_ISDIR(st.st_mode)) {
        puts("Cannot delete directory with rm command!");
        return;
    }
    unlink(filename);
}

void displayFile(char *filename) {
    char buffer[1024];
    int file = open(filename, O_RDONLY);

    if (file == -1 && strchr(filename, '/') == NULL) {
        size_t pathLen = strlen(filename) + 3;
        char *relativePath = (char *)malloc(pathLen);
        if (relativePath != NULL) {
            strcpy(relativePath, "./");
            strcat(relativePath, filename);
            file = open(relativePath, O_RDONLY);
            free(relativePath);
        }
    }

    if (file == -1) {
        puts("File does not exist!");
        return;
    }

    ssize_t n = 0;
    while ((n = read(file, buffer, sizeof(buffer))) > 0) {
        ssize_t written = 0;
        while (written < n) {
            ssize_t w = write(STDOUT_FILENO, buffer + written, (size_t)(n - written));
            if (w == -1) {
                puts("Failed while writing to stdout!");
                close(file);
                return;
            }
            written += w;
        }
    }
    
    if (n == -1) {
        puts("Failed while reading file!");
    }
    write(STDOUT_FILENO, "\n", 1);
    close(file);
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "command.h"
#include "string_parser.h"

int main(int argc, char *argv[]) {

    // Initial value setups
    int interactive_mode = 0;
    char *filename = NULL;
    FILE *input_stream = stdin;
    int output_fd = -1;
    char *line = NULL;
    size_t size = 0;
    ssize_t nread;
    char *command_idx[] = {"ls", "pwd", "mkdir", "cd", "cp", "mv", "rm", "cat"};

    //Mode detection and getting filename if in file mode
    if (argc == 1) {
        interactive_mode = 1;
    } else if (strcmp(argv[1], "-f") == 0 && argc == 3) {
        filename = argv[2];
    } else {
        exit(EXIT_FAILURE);
    }

    if (!interactive_mode) {
        input_stream = fopen(filename, "r");
        if (input_stream == NULL) {
            exit(EXIT_FAILURE);
        }

        output_fd = open("output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (output_fd == -1) {
            fclose(input_stream);
            exit(EXIT_FAILURE);
        }

        if (dup2(output_fd, STDOUT_FILENO) == -1) {
            close(output_fd);
            fclose(input_stream);
            exit(EXIT_FAILURE);
        }
        close(output_fd);
        setbuf(stdout, NULL);
    }

    //Main loop for interactive mode
    while (1) {
        if (interactive_mode) {
            fputs(">>> ", stdout);
        }

        nread = getline(&line, &size, input_stream);
        if (nread == -1) {
            if (!interactive_mode) {
                break;
            }
            free(line);
            exit(EXIT_FAILURE);
        }

        command_line cmd_line = str_tokenize(line);
        int should_exit = 0;
        int line_error = 0;
        for (int i = 0; i < cmd_line.num_token; i++) {
            char *params[10] = {NULL};
            char *command = NULL;
            char *token = NULL;
            char *saveptr = NULL;
            int arg_count = 0;

            command = strtok_r(cmd_line.command_list[i], " \t", &saveptr);
            if (command == NULL) {
                fputs("Error! Unrecognized command: ", stdout);
                puts("");
                line_error = 1;
                break;
            }

            params[arg_count++] = command;
            while ((token = strtok_r(NULL, " \t", &saveptr)) != NULL) {
                if (arg_count < (int)(sizeof(params) / sizeof(params[0]))) {
                    params[arg_count] = token;
                }
                arg_count++;
            }

            if (strcmp(command, "exit") == 0) {
                should_exit = 1;
                break;
            }

            int recognized = 0;
            for (int j = 0; j < (int)(sizeof(command_idx) / sizeof(command_idx[0])); j++) {
                if (strcmp(command, command_idx[j]) == 0) {
                    recognized = 1;

                    if (strcmp(command, "ls") == 0) {
                        if (arg_count != 1) {
                            fputs("Error! Unsupported parameters for command: ls\n", stdout);
                            line_error = 1;
                        } else {
                            listDir();
                        }
                    } else if (strcmp(command, "pwd") == 0) {
                        if (arg_count != 1) {
                            fputs("Error! Unsupported parameters for command: pwd\n", stdout);
                            line_error = 1;
                        } else {
                            showCurrentDir();
                        }
                    } else if (strcmp(command, "mkdir") == 0) {
                        if (arg_count < 2) {
                            fputs("Error! Missing parameters for command: mkdir\n", stdout);
                            line_error = 1;
                        } else if (arg_count > 2) {
                            fputs("Error! Unsupported parameters for command: mkdir\n", stdout);
                            line_error = 1;
                        } else {
                            makeDir(params[1]);
                        }
                    } else if (strcmp(command, "cd") == 0) {
                        if (arg_count < 2) {
                            fputs("Error! Missing parameters for command: cd\n", stdout);
                            line_error = 1;
                        } else if (arg_count > 2) {
                            fputs("Error! Unsupported parameters for command: cd\n", stdout);
                            line_error = 1;
                        } else {
                            changeDir(params[1]);
                        }
                    } else if (strcmp(command, "cp") == 0) {
                        if (arg_count < 3) {
                            fputs("Error! Missing parameters for command: cp\n", stdout);
                            line_error = 1;
                        } else if (arg_count > 3) {
                            fputs("Error! Unsupported parameters for command: cp\n", stdout);
                            line_error = 1;
                        } else {
                            copyFile(params[1], params[2]);
                        }
                    } else if (strcmp(command, "mv") == 0) {
                        if (arg_count < 3) {
                            fputs("Error! Missing parameters for command: mv\n", stdout);
                            line_error = 1;
                        } else if (arg_count > 3) {
                            fputs("Error! Unsupported parameters for command: mv\n", stdout);
                            line_error = 1;
                        } else {
                            moveFile(params[1], params[2]);
                        }
                    } else if (strcmp(command, "rm") == 0) {
                        if (arg_count < 2) {
                            fputs("Error! Missing parameters for command: rm\n", stdout);
                            line_error = 1;
                        } else if (arg_count > 2) {
                            fputs("Error! Unsupported parameters for command: rm\n", stdout);
                            line_error = 1;
                        } else {
                            deleteFile(params[1]);
                        }
                    } else if (strcmp(command, "cat") == 0) {
                        if (arg_count < 2) {
                            fputs("Error! Missing parameters for command: cat\n", stdout);
                            line_error = 1;
                        } else if (arg_count > 2) {
                            fputs("Error! Unsupported parameters for command: cat\n", stdout);
                            line_error = 1;
                        } else {
                            displayFile(params[1]);
                        }
                    }

                    break;
                }
            }

            if (!recognized) {
                fputs("Error! Unrecognized command: ", stdout);
                puts(command);
                line_error = 1;
            }

            if (line_error) {
                break;
            }
        }

        free_command_line(&cmd_line);
        if (should_exit) {
            free(line);
            if (!interactive_mode) {
                fclose(input_stream);
            }
            return 0;
        }
        if (line_error) {
            continue;
        }
    }

    free(line);
    if (!interactive_mode) {
        fputs("End of file\nBye Bye!\n", stdout);
        fclose(input_stream);
    }
    return 0;

    // Testing basic mode detection and file parsing
    // printf("Shell is in %s mode\n", interactive_mode ? "interactive" : "file");
    // if (!interactive_mode) {
    //     printf("Working with file: %s\n", filename);
    // }
}
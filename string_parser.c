#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "string_parser.h"

static char *trim(char *str) {
    char *end;
    while (*str && isspace((unsigned char)*str)) {
        str++;
    }

    if (*str == '\0') {
        return str;
    }

    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) {
        end--;
    }
    end[1] = '\0';
    return str;
}

command_line str_tokenize(char *str) {
    command_line result = {NULL, 0};
    char *copy = NULL;
    char *line = NULL;
    char *start = NULL;
    char *p = NULL;
    int count = 0;
    int idx = 0;

    if (str == NULL) {
        return result;
    }

    copy = (char *)malloc(strlen(str) + 1);
    if (copy == NULL) {
        return result;
    }
    strcpy(copy, str);

    line = strtok(copy, "\r\n");
    if (line == NULL) {
        line = copy;
        line[0] = '\0';
    }

    if (*trim(line) == '\0') {
        free(copy);
        return result;
    }

    count = 1;
    for (p = line; *p != '\0'; p++) {
        if (*p == ';') {
            count++;
        }
    }

    result.command_list = (char **)malloc((size_t)count * sizeof(char *));
    if (result.command_list == NULL) {
        free(copy);
        return result;
    }

    start = line;
    for (p = line;; p++) {
        if (*p == ';' || *p == '\0') {
            char saved = *p;
            char *segment;
            char *out_text;
            char *probe = NULL;
            char *saveptr = NULL;
            char *first_word = NULL;

            *p = '\0';
            segment = trim(start);

            probe = (char *)malloc(strlen(segment) + 1);
            if (probe == NULL) {
                int i;
                for (i = 0; i < idx; i++) {
                    free(result.command_list[i]);
                }
                free(result.command_list);
                free(copy);
                result.command_list = NULL;
                result.num_token = 0;
                return result;
            }
            strcpy(probe, segment);

            first_word = strtok_r(probe, " \t", &saveptr);
            out_text = (first_word == NULL) ? "" : segment;

            result.command_list[idx] = (char *)malloc(strlen(out_text) + 1);
            if (result.command_list[idx] == NULL) {
                int i;
                free(probe);
                for (i = 0; i < idx; i++) {
                    free(result.command_list[i]);
                }
                free(result.command_list);
                free(copy);
                result.command_list = NULL;
                result.num_token = 0;
                return result;
            }
            strcpy(result.command_list[idx], out_text);
            idx++;

            free(probe);

            if (saved == '\0') {
                break;
            }
            start = p + 1;
        }
    }

    result.num_token = idx;
    free(copy);
    return result;
}

void free_command_line(command_line *command) {
    if (command == NULL || command->command_list == NULL) {
        return;
    }

    for (int i = 0; i < command->num_token; i++) {
        free(command->command_list[i]);
    }
    free(command->command_list);
    command->command_list = NULL;
    command->num_token = 0;
}

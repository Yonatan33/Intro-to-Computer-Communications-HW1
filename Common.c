/* Ofir Yoffe - 303166318, Yonatan Gartenberg - 311126205 */

#define _CRT_SECURE_NO_WARNINGS

#include "Common.h"
#include <stdlib.h>
#include <string.h>

void parse_args(int argc, char *argv[], char **channel_ip, int *channel_port) {
    if (argc != 3) {
        printf("Sender/Channel Error! no. of arguments != 3");
        exit(EXIT_FAILURE);
    }

    *channel_ip = argv[1];
    *channel_port = strtol(argv[2], NULL, 10);
}

FILE *read_file_name_from_user(char *mode) {
    char input[MAX_INPUT] = { 0 };
    printf("enter file name:\n");
    int r = scanf("%99s", input);
    if (0 == strcmp(input, "quit")) {
        return NULL;
    } else {
        FILE *fp;
        fopen_s(&fp, input, mode);
        if (NULL == fp) {
            printf("File open failed with error: %d\n", errno);
        }
        return fp;
    }
}

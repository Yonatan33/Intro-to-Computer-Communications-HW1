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
    char input[MAX_INPUT] = {0};
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

void byte_to_bits(char byte, char *bits) {
    for (int i = 0; i < BITS_IN_BYTE; i++) {
        int bit = (byte >> (BITS_IN_BYTE - i - 1)) & 1;
        bits[i] = bit + '0';
    }
}

char bits_to_byte(const char *bits) {
    char res = 0;
    for (int i = 7; i >= 0; i--) {
        int bit = (bits[BITS_IN_BYTE - i - 1] - '0');
        res += bit << i;
    }
    return res;
}

void bit_array_to_packet(const char bits[ENCODED_BITS_IN_PACKET], packet_t *p, int encoded_bits) {
    for (int i = 0; i < ENCODED_BYTES_IN_PACKET; i++) {
        p->data[i] = bits_to_byte(&bits[i * BITS_IN_BYTE]);
    }
    p->encoded_bits = encoded_bits;
}

void packet_to_bit_array(packet_t *p, char bits[ENCODED_BITS_IN_PACKET]) {
    for (int i=0;i< ENCODED_BYTES_IN_PACKET;i++){
        byte_to_bits(p->data[i], &bits[i * BITS_IN_BYTE]);
    }
}
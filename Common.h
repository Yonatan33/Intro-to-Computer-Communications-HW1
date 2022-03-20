/* Ofir Yoffe - 303166318, Yonatan Gartenberg - 311126205 */

#ifndef Constants
#define Constants

#define MAX_INPUT 100
#define HAMMING_N 31
#define HAMMING_K 26
#define BITS_IN_BYTE 8

#define ENCODED_BYTES_IN_PACKET     HAMMING_N * HAMMING_K // divides in HAMMING_N , HAMMING_K
#define ENCODED_BITS_IN_PACKET      ENCODED_BYTES_IN_PACKET * BITS_IN_BYTE
#define DATA_BITS_IN_PACKET         ENCODED_BITS_IN_PACKET * HAMMING_K / HAMMING_N

#define DEBUG_ALL                   // TODO comment

#include <stdio.h>

typedef struct {
    int encoded_bits;
    char data[ENCODED_BYTES_IN_PACKET];
} packet_t;

void parse_args(int argc, char *argv[], char **channel_ip, int *channel_port);

FILE *read_file_name_from_user(char *mode);

void byte_to_bits(char byte, char *bits);

char bits_to_byte(const char *bits);

void bit_array_to_packet(const char bits[ENCODED_BITS_IN_PACKET], packet_t *p, int encoded_bits);

void packet_to_bit_array(packet_t *p, char bits[ENCODED_BITS_IN_PACKET]);

#endif

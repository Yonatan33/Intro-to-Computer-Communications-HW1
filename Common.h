/* Ofir Yoffe - 303166318, Yonatan Gartenberg - 311126205 */

#ifndef Constants
#define Constants

#define MAX_INPUT 100
#define HAMMING_N 31
#define HAMMING_K 26

#define BITS_IN_BYTE 8
#define BLOCK_BYTE_SIZE 4
#define BLOCKS_IN_PACKET 256 // BLOCKS_IN_PACKET * BLOCK_BYTE_SIZE ~ 1000 BYTES (Avg. Packet Size)

#define DEBUG_ALL                   // TODO comment

#include <stdio.h>

typedef struct {
    char blocks[BLOCKS_IN_PACKET][BLOCK_BYTE_SIZE];
    int valid_blocks;
} packet_t;

void parse_args(int argc, char *argv[], char **channel_ip, int *channel_port);

FILE *read_file_name_from_user(char *mode);

void byte_to_bits(char byte, char *bits);

char bits_to_byte(const char *bits);

void bit_array_to_packet(const char bits[BLOCKS_IN_PACKET][HAMMING_N], packet_t *p, int valid_blocks);

#endif

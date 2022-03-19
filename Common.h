/* Ofir Yoffe - 303166318, Yonatan Gartenberg - 311126205 */

#ifndef Constants
#define Constants

#define MAX_INPUT 100
#define HAMMING_N 31
#define HAMMING_K 26
#define BLOCKS_IN_TCP_PACKET 256 // a block is 4 bytes containing 31 data+hamming bits

#define DEBUG_ALL // TODO comment

#include <stdio.h>

void parse_args(int argc, char *argv[], char **channel_ip, int *channel_port);

FILE *read_file_name_from_user(char *mode);

#endif

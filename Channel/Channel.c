/* Ofir Yoffe - 303166318, Yonatan Gartenberg - 311126205 */
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include "../Utils/Common.h"
#include "../Utils/WinSock_handlers.h"
#define RAND_MAX 4294967296

#pragma comment(lib, "Ws2_32.lib")

void parse_channel_args(int argc, char *argv[], bool *flag_random, int *n, int *seed) {
    if (argc < 3 || argc > 4) {
        printf("Sender Error! no. of arguments < 2 or > 3");
        exit(EXIT_FAILURE);
    }

    if (0 == strcmp(argv[1], "-r")) {
        *flag_random = true;
        *n = strtol(argv[2], NULL, 10);
        *seed = strtol(argv[3], NULL, 10);
    } else {
        *flag_random = false;
        *n = strtol(argv[2], NULL, 10);
    }
}

int read_continue_from_user() {
    char input[MAX_INPUT];
    printf("continue? (yes/no)\n");
    scanf("%99s", input);
    return strcmp(input, "no");
}



/* We implemented a more efficient approch to generating random numbers.
* Because RAND_MAX is only 2^15 and 16 random bit are needed, we generate 30 random bits and keep the spare ones
* to use in the next 15 bit random generation. Thus, we need 2 random generations every 15 bits of message
* In total - 16 random generations for 15 bits of message, rather than 30, saving ~47% of random generations
*/
int noise(const char *buf_input, char *buf_output, bool random, int n, int seed, int* packets_count, int* next_flip) { // TODO fix according to noise
    memset(buf_output, 0, HAMMING_N); // zero output buffer
    memcpy(buf_output, buf_input, HAMMING_N);
    printf("entered noise function");
    int flipped_bits = 0;
    if (random) {                       // random flip
        unsigned int coin;        
        int mask = pow(2, 16) - 1;
        unsigned int large_random;
        int spare_bits = 0;
        printf("Binary mask: %d\n", mask);
        for (int i = 0; i < HAMMING_N; i++) {
            if (spare_bits == 0) {    // need to generate 2 new random - 30 bits               
                large_random = rand();
                large_random = large_random << 15;
                large_random += rand();                
                coin = large_random & mask;
                large_random = large_random >> 16;
                spare_bits = 14;
            }
            else {                     // need to generate 1 new random - 15 + spare bits from prior random number
                large_random = large_random << 15;
                large_random += rand();
                coin = large_random & mask;
                large_random = large_random >> 16;
                spare_bits--;               
            }
            printf("large random: %u\n", large_random);
            printf("Coin used: %u\n", coin);
            if (coin <= n) {
                buf_output[i] = 97 - buf_output[i];  // ascii equivalent to integer "1-buf_output[i]". i.e, flip the bit
                printf("Checked %u coin against %d probability\n", coin, n);
                flipped_bits++;
            }
        }        
    }
    else {  // deterministic flip 
        for (int i = *next_flip; HAMMING_N * (*packets_count - 1) <= i && i < HAMMING_N *  (*packets_count); i += n) {            
            buf_output[i % HAMMING_N] = 97 - buf_output[i % HAMMING_N]; // flip the bit
            *next_flip += n;
            flipped_bits++;
        }        
        *packets_count += 1;
    }
    return flipped_bits;
}

void main_loop(SOCKET *socket_sender, SOCKET *socket_receiver, bool random, int n, int seed) {
    char buf_input[HAMMING_N] = {0};
    char buf_output[HAMMING_N] = {0};

    size_t total_retransmit_size = 0;
    size_t recv_size, retransmit_size;
    //int coin_tosses = 0;
    //UINT32 coin;
    int packets_count = 1;
    int next_flip = n;
    int flipped = 0;
    srand(seed);
    while (0 < (recv_size = s_recv(socket_sender, buf_input, HAMMING_N))) { // loop until sender socket closes
        flipped += noise(buf_input, buf_output, random, n, seed, &packets_count, &next_flip); // generate noise
        printf("Bytes received: %d, %.*s %.*s\n", recv_size, HAMMING_N, buf_input, HAMMING_N, buf_output); // TODO erase

        if (SOCKET_ERROR == (retransmit_size = s_send(socket_receiver, buf_output, HAMMING_N))) {
            printf("Sending failed with error: %d\n", WSAGetLastError());
            break;
        } else {
            total_retransmit_size += retransmit_size;
        }

        memset(buf_input, 0, HAMMING_N);
        
    }
    if (recv_size < 0) { // sender socket error
        printf("Receive failed with error: %d\n", WSAGetLastError());
    }

    printf("retransmitted %d bytes, flipped %d bits\n", total_retransmit_size / 8, flipped);
}

int main(int argc, char *argv[]) {
    WSADATA wsaData;

    bool random;
    int n, seed;

    parse_channel_args(argc, argv, &random, &n, &seed);

    s_startup(&wsaData);

    SOCKET socket_listen, socket_sender, socket_recv;
    s_socket(&socket_listen);
    s_bind(&socket_listen, "127.0.0.1", 0); // TODO get self ip
    s_listen(&socket_listen);

    printf("sender socket: ");
    s_print(&socket_listen);

    printf("receiver socket: ");
    s_print(&socket_listen); // TODO port doesn't change because listen puts multiple sockets on the same port

    do {
        s_accept(&socket_listen, &socket_sender);
        s_accept(&socket_listen, &socket_recv);

        main_loop(&socket_sender, &socket_recv, random, n, seed);

        s_shutdown(&socket_sender, SD_BOTH);
        s_shutdown(&socket_recv, SD_BOTH);
        s_close(&socket_sender);
        s_close(&socket_recv);
    } while (read_continue_from_user()); // loop until "no"

    s_close(&socket_listen); // No need to listen to other connections

    s_cleanup();

    return EXIT_SUCCESS;
}
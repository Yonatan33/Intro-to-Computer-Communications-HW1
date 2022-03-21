/* Ofir Yoffe - 303166318, Yonatan Gartenberg - 311126205 */

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>
#include <stdbool.h>
#include "../Common.h"
#include "../WinSock_handlers.h"

#pragma comment(lib, "Ws2_32.lib")

void parse_channel_args(int argc, char *argv[], bool *flag_random, int *n, int *seed) {
    if (argc < 3 || argc > 4) {
        printf("Channel Error! no. of arguments < 2 or > 3");
        exit(EXIT_FAILURE);
    }

    if (0 == strcmp(argv[1], "-r")) {
        if (argc < 4 || argc > 4) {
            printf("Channel Error! no. of arguments != 3");
            exit(EXIT_FAILURE);
        }

        *flag_random = true;
        *n = strtol(argv[2], NULL, 10);
        *seed = strtol(argv[3], NULL, 10);
    } else {
        if (argc < 3 || argc > 3) {
            printf("Channel Error! no. of arguments != 2");
            exit(EXIT_FAILURE);
        }

        *flag_random = false;
        *n = strtol(argv[2], NULL, 10);
    }
}

int read_continue_from_user() {
    char input[MAX_INPUT] = {0};
    printf("continue? (yes/no)\n");
    int r = scanf("%99s", input);
    return strcmp(input, "no");
}

void flip_bit(char *pt) {
    if (*pt == '1')
        *pt = '0';
    else
        *pt = '1';
}

int noise(char *receiver_bits, int receiver_bits_len, int *det_count, bool random, int n) {
    int flipped_bits = 0;
    if (random) { // random flip
        for (int i = 0; i < receiver_bits_len; i++) {
            int rand_num = rand() % 32768; // generates a number in [0,RAND_MAX], 32767 (2^15-1) is default RAND_MAX
            bool flip = ((rand_num <= (n / 2)) ? true : false); // n/2 to get n/(2^16) instead of (2^15)
            if (flip) {
                flip_bit(&(receiver_bits[i]));
                flipped_bits++;
            }
        }

    } else {  // deterministic flip
        for (int i = 0; i < receiver_bits_len; i++) {
            (*det_count)++;
            if (*det_count == n) {
                flip_bit(&(receiver_bits[i]));
                *det_count = 0;
                flipped_bits++;
            }
        }
    }
    return flipped_bits;
}

void main_loop(SOCKET *socket_sender, SOCKET *socket_receiver, bool random, int n) {
    size_t recv_size;
    int total_bits_retransmitted = 0, bits_flipped = 0;
    int det_count = 0;

    packet_t p_in = {0}, p_out = {0};
    char encoded_data_bits[ENCODED_BITS_IN_PACKET] = {0};
    char noised_data_bits[ENCODED_BITS_IN_PACKET] = {0};

    while (0 < (recv_size = s_recv(socket_sender, (char *) &p_in, sizeof(p_in)))) { // loop until sender socket closes
        packet_to_bit_array(&p_in, encoded_data_bits);
        memcpy(noised_data_bits, encoded_data_bits, p_in.encoded_bits);
        bits_flipped += noise(noised_data_bits, p_in.encoded_bits, &det_count, random, n); // generate noise
        bit_array_to_packet(noised_data_bits, &p_out, p_in.encoded_bits);

#ifdef DEBUG_ALL
        for (int i = 0; i < p_in.encoded_bits / HAMMING_N; i++) {
            printf("Data Bits Sent: %.*s %.*s\n", HAMMING_N, &encoded_data_bits[i * HAMMING_N], HAMMING_N,
                   &noised_data_bits[i * HAMMING_N]);
        }
#endif

        if (SOCKET_ERROR == s_send(socket_receiver, (char *) &p_out, sizeof(p_out))) {
            printf("Sending failed with error: %d\n", WSAGetLastError());
            break;
        } else {
            total_bits_retransmitted += p_out.encoded_bits;
        }
    }
    if (recv_size < 0) { // sender socket error
        printf("Receive failed with error: %d\n", WSAGetLastError());
    }

    printf("retransmitted %d bytes, flipped %d bits\n", total_bits_retransmitted / BITS_IN_BYTE, bits_flipped);
}

int main(int argc, char *argv[]) {
    WSADATA wsaData;

    bool random;
    int n, seed;

    parse_channel_args(argc, argv, &random, &n, &seed);
    if (random) srand(seed);

    s_startup(&wsaData);
    char *my_ip = get_my_ip();

    SOCKET socket_listen_sender, socket_listen_recv, socket_sender, socket_recv;

    s_socket(&socket_listen_sender);
    s_socket(&socket_listen_recv);

    s_bind(&socket_listen_sender, my_ip, 0);
    s_bind(&socket_listen_recv, my_ip, 0);

    s_listen(&socket_listen_sender);
    s_listen(&socket_listen_recv);

    printf("sender socket: ");
    s_print(&socket_listen_sender);
    printf("receiver socket: ");
    s_print(&socket_listen_recv);

    do {
        s_accept(&socket_listen_sender, &socket_sender);
        s_accept(&socket_listen_recv, &socket_recv);

        main_loop(&socket_sender, &socket_recv, random, n);

        s_shutdown(&socket_sender, SD_BOTH);
        s_shutdown(&socket_recv, SD_BOTH);
        s_close(&socket_sender);
        s_close(&socket_recv);

    } while (read_continue_from_user()); // loop until "no"

    s_close(&socket_listen_sender);
    s_close(&socket_listen_recv);

    s_cleanup();

    return EXIT_SUCCESS;
}
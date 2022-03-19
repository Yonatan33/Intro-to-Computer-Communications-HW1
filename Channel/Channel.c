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
    char input[MAX_INPUT] = { 0 };
    printf("continue? (yes/no)\n");
    int r = scanf("%99s", input);
    return strcmp(input, "no");
}

int noise(const char *buf_input, char *buf_output, bool random, int n, int seed) { // TODO fix according to noise
    memset(buf_output, 0, HAMMING_N); // zero output buffer
    memcpy(buf_output, buf_input, HAMMING_N);
    return 0; // flipped bits
}

void main_loop(SOCKET *socket_sender, SOCKET *socket_receiver, bool random, int n, int seed) {
    char buf_input[HAMMING_N] = {0};
    char buf_output[HAMMING_N] = {0};

    size_t total_retransmit_size = 0;
    size_t recv_size, retransmit_size;

    while (0 < (recv_size = s_recv(socket_sender, buf_input, HAMMING_N))) { // loop until sender socket closes
        noise(buf_input, buf_output, random, n, seed); // generate noise
        printf("Bytes received: %d, %.*s %.*s\n", (int)recv_size, (int)HAMMING_N, buf_input, HAMMING_N, buf_output); // TODO erase

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

    printf("retransmitted %d bytes, flipped %d bits\n", (int)total_retransmit_size / 8, (int)0);
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
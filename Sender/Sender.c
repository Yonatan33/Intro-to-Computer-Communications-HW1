/* Ofir Yoffe - 303166318, Yonatan Gartenberg - 311126205 */

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>
#include <math.h>
#include "../Common.h"
#include "../WinSock_handlers.h"

#pragma comment(lib, "Ws2_32.lib")

void encode(const char *input_ptr, char *output_ptr) {
    memset(output_ptr, '0', HAMMING_N); // zero output buffer

    // copy input to output, skipping hamming bits (indexes 1,2,4,8,16)
    for (int input_i = 1, output_i = 1; output_i <= HAMMING_N; output_i++) {
        if (output_i == 1 || output_i == 2 || output_i == 4 || output_i == 8 || output_i == 16) {
            continue;
        }
        output_ptr[output_i - 1] = input_ptr[input_i - 1];
        input_i++;
    }

    // calculate hamming bits
    for (int output_i = 1; output_i <= HAMMING_N; output_i++) {
        if ('1' == output_ptr[output_i - 1]) { // xor with hamming bits
            if (1 == ((output_i) & 1))
                output_ptr[0] = ('0' == output_ptr[0]) ? '1' : '0';
            if (2 == ((output_i) & 2))
                output_ptr[1] = ('0' == output_ptr[1]) ? '1' : '0';
            if (4 == ((output_i) & 4))
                output_ptr[3] = ('0' == output_ptr[3]) ? '1' : '0';
            if (8 == ((output_i) & 8))
                output_ptr[7] = ('0' == output_ptr[7]) ? '1' : '0';
            if (16 == ((output_i) & 16))
                output_ptr[15] = ('0' == output_ptr[15]) ? '1' : '0';
        }
    }
}

void send_packet(SOCKET *socket, char data_bits[DATA_BITS_IN_PACKET], int data_bits_len) {
    char encoded_data_bits[ENCODED_BITS_IN_PACKET] = {0};

    // encode
    for (int i = 0; i < data_bits_len / HAMMING_K; i++) {
        encode(&data_bits[i * HAMMING_K], &encoded_data_bits[i * HAMMING_N]);
    }

    // resize
    packet_t p;
    bit_array_to_packet(encoded_data_bits, &p, data_bits_len * HAMMING_N / HAMMING_K);

    // send
    if (SOCKET_ERROR == s_send(socket, (char *) &p, sizeof(p))) {
        fprintf(stderr,"Sending failed with error: %d\n", WSAGetLastError());
    } else {
#ifdef DEBUG_ALL
        for (int i = 0; i < data_bits_len / HAMMING_K; i++) {
            printf("Data Bits Sent: %.*s %.*s\n", HAMMING_K, &data_bits[i * HAMMING_K], HAMMING_N,
                   &encoded_data_bits[i * HAMMING_N]);
        }
#endif
    }
}


static void send_file(SOCKET *socket, FILE *fp) {
    int file_byte_length = 0, total_bits_sent = 0;

    char data_bits[DATA_BITS_IN_PACKET] = {0};

    char buf;
    int bit_index = 0;
    while (0 < fread(&buf, 1, 1, fp)) {
        file_byte_length++;
        byte_to_bits(buf, ((char *) data_bits) + bit_index);
        bit_index += BITS_IN_BYTE;

        if (bit_index == DATA_BITS_IN_PACKET) {
            send_packet(socket, data_bits, bit_index);
            memset(data_bits, 0, DATA_BITS_IN_PACKET);
            total_bits_sent += bit_index * HAMMING_N / HAMMING_K;
            bit_index = 0;
        }
    }
    if (bit_index > 0) { // send last (and maybe) partial packet
        send_packet(socket, data_bits, bit_index);
        total_bits_sent += bit_index * HAMMING_N / HAMMING_K;
    }

    fclose(fp);
    printf("file length: %d bytes\n", file_byte_length);
    printf("sent: %d bytes\n", total_bits_sent / BITS_IN_BYTE);
}

int main(int argc, char *argv[]) {
    WSADATA wsaData;
    SOCKET socket;
    FILE *fp;
    char *channel_ip;
    int channel_port;

    parse_args(argc, argv, &channel_ip, &channel_port);
    s_startup(&wsaData);
    s_socket(&socket);
    s_connect(&socket, channel_ip, channel_port);

    while (NULL != (fp = read_file_name_from_user("r"))) { // loop until "quit"
        send_file(&socket, fp);
        s_shutdown(&socket, SD_BOTH);
        s_close(&socket);
        s_socket(&socket);
        s_connect(&socket, channel_ip, channel_port);
    }

    s_shutdown(&socket, SD_BOTH);
    s_close(&socket);
    s_cleanup();
    return EXIT_SUCCESS;
}

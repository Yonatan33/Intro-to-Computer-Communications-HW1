/* Ofir Yoffe - 303166318, Yonatan Gartenberg - 311126205 */

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>
#include "../Common.h"
#include "../WinSock_handlers.h"

#pragma comment(lib, "Ws2_32.lib")

int check_parity(const char *buf_input, int len, int index) {
    char parity = buf_input[index - 1];
    for (int i = index + 1; i <= len; i++) {
        if ((i & index) == index) { // check only relevant indexes
            if (buf_input[i - 1] == '1') // flip parity if needed
                parity = ('0' == parity) ? '1' : '0';
        }
    }
    return parity - '0';
}

int decode(const char buf_input[HAMMING_N], char buf_output[HAMMING_K]) {
    char buf_input_temp[HAMMING_N];
    memcpy(buf_input_temp, buf_input, HAMMING_N);
    memset(buf_output, 0, HAMMING_K); // zero output buffer

    // calculate error index
    int error_index = 0;
    for (int i = 0; i <= 4; i++) {
        error_index |= check_parity(buf_input_temp, HAMMING_N, 1 << i) << i;
    }

    // correct if needed (error_index != 0)
    if (0 != error_index) {
        buf_input_temp[error_index - 1] = (buf_input_temp[error_index - 1] == '0') ? '1' : '0';
    }

    // copy input to output, skipping hamming bits
    for (int input_i = 1, output_i = 1; input_i <= HAMMING_N; input_i++) {
        if (input_i == 1 || input_i == 2 || input_i == 4 || input_i == 8 || input_i == 16) {
            continue;
        }
        buf_output[output_i - 1] = buf_input_temp[input_i - 1];
        output_i++;
    }

    return error_index != 0;
}

int write_to_file(FILE *fp, const char *decoded_data_bits, int encoded_bits) {
    int total_write_size = 0;
    char bits_buf[8];
    int next_bit = 0;

    for (int i = 0; i < encoded_bits; i++) {
        bits_buf[next_bit] = decoded_data_bits[i];
        next_bit++;
        if (next_bit == 8) {
            char next_byte = bits_to_byte(bits_buf);
            total_write_size += (int) fwrite(&next_byte, 1, 1, fp);
            next_bit = 0;
        }
    }
    return total_write_size;
}

static void recv_file(SOCKET *socket, FILE *fp) {
    size_t total_bits_recv = 0, total_byte_written = 0, total_errors_corrected = 0;
    size_t recv_size;

    packet_t p_in = {0};
    char noised_data_bits[ENCODED_BITS_IN_PACKET] = {0};
    char decoded_data_bits[ENCODED_BITS_IN_PACKET] = {0};

    while (0 < (recv_size = s_recv(socket, (char *) &p_in, sizeof(p_in)))) {
        total_bits_recv += p_in.encoded_bits / BITS_IN_BYTE;
        packet_to_bit_array(&p_in, noised_data_bits);
        for (int i = 0; i < p_in.encoded_bits / HAMMING_N; i++) {
            total_errors_corrected += decode(&noised_data_bits[i * HAMMING_N],
                                             &decoded_data_bits[i * HAMMING_K]); // decode with hamming
        }

#ifdef DEBUG_ALL
        for (int i = 0; i < p_in.encoded_bits / HAMMING_N; i++) {
            printf("Data Bits Received: %.*s %.*s\n", HAMMING_N, &noised_data_bits[i * HAMMING_N], HAMMING_K,
                   &decoded_data_bits[i * HAMMING_K]);
        }
#endif

        // write to file
        total_byte_written += write_to_file(fp, decoded_data_bits, p_in.encoded_bits * HAMMING_K / HAMMING_N);
    }
    if (SOCKET_ERROR == recv_size) {
        printf("Sending failed with error: %d\n", WSAGetLastError());
    }
    fclose(fp);
    printf("received: %d bytes\n", (int) total_bits_recv);
    printf("wrote: %d bytes\n", (int) total_byte_written);
    printf("corrected %d errors\n", (int) total_errors_corrected);
}

int main(int argc, char *argv[]) {
    setbuf(stdout, 0);
    WSADATA wsaData;
    SOCKET socket;
    FILE *fp;
    char *channel_ip;
    int channel_port;

    parse_args(argc, argv, &channel_ip, &channel_port);
    s_startup(&wsaData);
    s_socket(&socket);
    s_connect(&socket, channel_ip, channel_port);

    while (NULL != (fp = read_file_name_from_user("w"))) { // loop until "quit"
        recv_file(&socket, fp);
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

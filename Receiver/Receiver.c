/* Ofir Yoffe - 303166318, Yonatan Gartenberg - 311126205 */
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>
#include "../Utils/Common.h"
#include "../Utils/WinSock_handlers.h"

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

int decode(char *buf_input, char *buf_output) {
    memset(buf_output, 0, HAMMING_K); // zero output buffer

    // calculate error index
    int error_index = 0;
    for (int i=0;i<=4;i++){
        error_index |= check_parity(buf_input, HAMMING_N, 1 << i) << i;
    }

    // correct if needed (error_index != 0)
    if (0 != error_index) {
        buf_input[error_index - 1] = (buf_input[error_index - 1] == '0') ? '1' : '0';
    }

    // copy input to output, skipping hamming bits
    for (int input_i = 1, output_i = 1; input_i <= HAMMING_N; input_i++) {
        if (input_i == 1 || input_i == 2 || input_i == 4 || input_i == 8 || input_i == 16) {
            continue;
        }
        buf_output[output_i - 1] = buf_input[input_i - 1];
        output_i++;
    }

    return error_index != 0;
}

char bits_to_byte(const char *bits) {
    char res = 0;
    for (int i = 7; i >= 0; i--) {
        res += (char) ((bits[7 - i] - '0') << i);
    }
    return res;
}

static void recv_file(SOCKET *socket, FILE *fp) {
    char buf_input[HAMMING_N] = {0};
    char buf_output[HAMMING_K] = {0};
    size_t total_recv_size = 0, total_write_size = 0, total_corrected = 0;
    size_t recv_size;
    char bits_buf[8];
    int next_bit = 0;

    while (0 < (recv_size = s_recv(socket, buf_input, HAMMING_N))) {
        printf("Bytes received: %d, %.*s\n", recv_size, HAMMING_N, buf_input); // TODO erase
        total_corrected += decode(buf_input, buf_output); // decode with hamming
        printf("Bytes received: %d, %.*s %.*s\n", recv_size, HAMMING_N, buf_input, HAMMING_K, buf_output); // TODO erase

        // write to file
        for (int i = 0; i < HAMMING_K; i++) {
            bits_buf[next_bit] = buf_output[i];
            next_bit++;
            if (next_bit == 8) {
                char next_byte = bits_to_byte(bits_buf);
                total_write_size += fwrite(&next_byte, 1, 1, fp);
                next_bit = 0;
            }
        }

        total_recv_size += recv_size;
        memset(buf_input, 0, HAMMING_N);
    }
    if (SOCKET_ERROR == recv_size) {
        printf("Sending failed with error: %d\n", WSAGetLastError());
    }
    fclose(fp);
    printf("received: %d bytes\n", total_recv_size / 8);
    printf("wrote: %d bytes\n", total_write_size);
    printf("corrected %d errors\n", total_corrected);
}

int main(int argc, char *argv[]) {
    setbuf( stdout, 0);
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

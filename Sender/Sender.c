/* Ofir Yoffe - 303166318, Yonatan Gartenberg - 311126205 */

#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>
#include <math.h>
#include "../Utils/Common.h"
#include "../Utils/WinSock_handlers.h"


#pragma comment(lib, "Ws2_32.lib")

void encode(const char *input_ptr, char *buf_output) {
    memset(buf_output, '0', HAMMING_N); // zero output buffer

    // copy input to output, skipping hamming bits (indexes 1,2,4,8,16)
    for (int input_i = 1, output_i = 1; output_i <= HAMMING_N; output_i++) {
        if (output_i == 1 || output_i == 2 || output_i == 4 || output_i == 8 || output_i == 16) {
            continue;
        }
        buf_output[output_i - 1] = input_ptr[input_i - 1];
        input_i++;
    }

    // calculate hamming bits
    for (int output_i = 1; output_i <= HAMMING_N; output_i++) {
        if ('1' == buf_output[output_i - 1]) { // xor with hamming bits
            if (1 == ((output_i) & 1))
                buf_output[0] = ('0' == buf_output[0]) ? '1' : '0';
            if (2 == ((output_i) & 2))
                buf_output[1] = ('0' == buf_output[1]) ? '1' : '0';
            if (4 == ((output_i) & 4))
                buf_output[3] = ('0' == buf_output[3]) ? '1' : '0';
            if (8 == ((output_i) & 8))
                buf_output[7] = ('0' == buf_output[7]) ? '1' : '0';
            if (16 == ((output_i) & 16))
                buf_output[15] = ('0' == buf_output[15]) ? '1' : '0';
        }
    }
}

char *file_to_bits(FILE *fp, int *size) {
    // find out file size (int bytes)
    fseek(fp, 0L, SEEK_END);
    *size = ftell(fp) * 8;
    fseek(fp, 0L, SEEK_SET);

    // convert file bits to bytes
    char *file_bits = malloc(*size);
    char buf;
    while (0 < fread(&buf, 1, 1, fp)) {
        for (int i = 7; i >= 0; i--) {
            int current_bit_index = (ftell(fp) - 1) * 8 + (7 - i);
            int current_bit = (buf >> i) & 1;
            file_bits[current_bit_index] = (char) (current_bit + '0');
        }
    }
    return file_bits;
}

static void send_file(SOCKET *socket, FILE *fp) {
    char buf_output[HAMMING_N] = {0};
    int file_size_in_bits = 0, total_sent_size = 0;
    int sent_size;

    char *file_bits = file_to_bits(fp, &file_size_in_bits);

    for (int i = 0; i < file_size_in_bits; i += HAMMING_K) {
        encode(file_bits + i, buf_output); // encode with hamming
        printf("Bytes received: %.*s %.*s\n", HAMMING_K, file_bits + i, HAMMING_N, buf_output); // TODO erase
        if (SOCKET_ERROR == (sent_size = s_send(socket, buf_output, HAMMING_N))) {
            printf("Sending failed with error: %d\n", WSAGetLastError());
            break;
        }
        total_sent_size += sent_size;
    }
    fclose(fp);
    printf("file length: %d bytes\n", file_size_in_bits / 8);
    printf("sent: %d bytes\n", total_sent_size / 8);
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

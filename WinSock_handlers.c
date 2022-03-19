/* Ofir Yoffe - 303166318, Yonatan Gartenberg - 311126205 */

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <WinSock2.h>
#include <stdio.h>
#include "WinSock_handlers.h"

void s_print(const SOCKET *s) {
    struct sockaddr_in sock_addr;
    int sock_addr_len = sizeof(sock_addr);
    getsockname(*s, (struct sockaddr *) &sock_addr, &sock_addr_len);

    printf("IP address = %s port = %d\n", inet_ntoa(sock_addr.sin_addr), ntohs(sock_addr.sin_port));
}

void s_startup(WSADATA *wsaData) {
    int err = WSAStartup(MAKEWORD(2, 2), wsaData);
    if (NO_ERROR != err) {
        printf("WSAStartup failed with error: %d\n", err);
        exit(EXIT_FAILURE);
    }
}

void s_cleanup() {
    if (SOCKET_ERROR == WSACleanup()) {
        printf("Cleanup socket failed with error: %d\n", WSAGetLastError());
        exit(EXIT_FAILURE);
    }
}

void s_socket(SOCKET *s) {
    *s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (INVALID_SOCKET == *s) {
        printf("Socket creation failed with error: %d\n", WSAGetLastError());
        s_cleanup();
        exit(EXIT_FAILURE);
    }
}

void s_close(const SOCKET *socket) {
    if (SOCKET_ERROR == closesocket(*socket)) {
        printf("Closing socket failed with error: %d\n", WSAGetLastError());
        s_cleanup();
        exit(EXIT_FAILURE);
    }
}

void s_connect(SOCKET *socket, char *dest_ip, u_short dest_port) {
    struct sockaddr_in dest_addr = {
            .sin_family = AF_INET,
            .sin_addr.s_addr = inet_addr(dest_ip),
            .sin_port = htons(dest_port)
    };

    if (SOCKET_ERROR == connect(*socket, (SOCKADDR *) &dest_addr, sizeof(dest_addr))) {
        printf("Connection failed with error: %d\n", WSAGetLastError());
        s_close(socket);
        s_cleanup();
        exit(EXIT_FAILURE);
    }
}

void s_bind(SOCKET *socket_listen, char *host_ip, u_short host_port) {
    struct sockaddr_in server_addr = {
            .sin_family = AF_INET,
            .sin_addr.s_addr = inet_addr(host_ip),
            .sin_port = htons(host_port)
    };

    if (SOCKET_ERROR == bind(*socket_listen, (SOCKADDR *) &server_addr, sizeof(server_addr))) {
        printf("Bind failed with error: %d\n", WSAGetLastError());
        s_close(socket_listen);
        s_cleanup();
        exit(EXIT_FAILURE);
    }
}

void s_listen(SOCKET *socket_listen) {
    if (SOCKET_ERROR == listen(*socket_listen, 2)) {
        printf("Listen failed with error: %d\n", WSAGetLastError());
        s_close(socket_listen);
        s_cleanup();
        exit(EXIT_FAILURE);
    }
}

void s_accept(SOCKET *socket_listen, SOCKET *socket_client) {
    *socket_client = accept(*socket_listen, NULL, NULL);
    if (INVALID_SOCKET == *socket_client) {
        printf("Accept failed with error: %d\n", WSAGetLastError());
        s_close(socket_listen);
        s_cleanup();
        exit(EXIT_FAILURE);
    }
}

int s_send(const SOCKET *socket, char *send_buf, int send_buf_len) {
    return send(*socket, send_buf, send_buf_len, 0);
}

int s_recv(const SOCKET *socket, char *recv_buf, int recv_buf_len) {
    return recv(*socket, recv_buf, recv_buf_len, 0);
}

void s_shutdown(SOCKET *socket, int how) {
    if (SOCKET_ERROR == shutdown(*socket, how)) {
        printf("Shutdown failed with error: %d\n", WSAGetLastError());
        s_close(socket);
        s_cleanup();
        exit(EXIT_FAILURE);
    }
}

char *get_my_ip() {
    char hostbuffer[256];
    struct hostent *host_entry;

    // To retrieve hostname
    gethostname(hostbuffer, sizeof(hostbuffer));

    // To retrieve host information
    host_entry = gethostbyname(hostbuffer);

    // To convert an Internet network
    // address into ASCII string
    return inet_ntoa(*((struct in_addr *)
            host_entry->h_addr_list[0]));
}
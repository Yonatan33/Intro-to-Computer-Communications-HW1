/* Ofir Yoffe - 303166318, Yonatan Gartenberg - 311126205 */

#ifndef WINSOCK_HANDLERS
#define WINSOCK_HANDLERS

void s_print(const SOCKET *s);

void s_startup(WSADATA *wsaData);

void s_cleanup();

void s_socket(SOCKET *s);

void s_close(const SOCKET *socket);

void s_connect(SOCKET *socket, char *dest_ip, u_short dest_port);

void s_bind(SOCKET *socket_listen, char *host_ip, u_short host_port);

void s_listen(SOCKET *socket_listen);

void s_accept(SOCKET *socket_listen, SOCKET *socket_client);

int s_send(const SOCKET *socket, char *send_buf, int send_buf_len);

int s_recv(const SOCKET *socket, char *recv_buf, int recv_buf_len);

void s_shutdown(SOCKET *socket, int how);

#endif

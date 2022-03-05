/* Ofir Yaffe - , Yonatan Gartenberg - 311126205 */
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
#include <WinSock2.h>
#include "../Utils/Constants.h"


#pragma comment(lib, "Ws2_32.lib")
#define REMOTE_HOST_IP "127.0.0.1" 
#define IN_PORT 6342 

int main(int argc, char* argv[])
{
	SOCKET TCPClientSocket;
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR)
	{
		printf("Error at WSAStartup()\n");
	}
		
	TCPClientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (TCPClientSocket == INVALID_SOCKET)
	{
		printf("Scoket creation failed with error: %d", WSAGetLastError());
	}

	struct sockaddr_in remote_addr;

	remote_addr.sin_family = AF_INET;
	remote_addr.sin_addr.s_addr = inet_addr(REMOTE_HOST_IP);
	remote_addr.sin_port = htons(IN_PORT);

	int status = connect(TCPClientSocket, (SOCKADDR*)&remote_addr, sizeof(remote_addr));
	if (status == SOCKET_ERROR)
	{
		printf("Connection failed with error: %d", WSAGetLastError());
	}

	int iReceive;
	char receive_buf[MSG_SIZE];
	int iReceiveBuf = strlen( receive_buf) + 1;
	iReceive = recv(TCPClientSocket, receive_buf, iReceiveBuf, 0);

	if (iReceive == SOCKET_ERROR)
	{
		printf("Receive data failed with error: %d", WSAGetLastError());
	}


	char send_buf[MSG_SIZE] = "TESTTEST";
	int iSendBuf = strlen(send_buf);
	/*debug*/
	/*strcpy(send_buf, "TEST");*/

	int iSend = send(TCPClientSocket, send_buf, MSG_SIZE, 0);
	if (iSend == SOCKET_ERROR)
	{
		printf("Sending to server failed with error: %d", WSAGetLastError());
	}

	int iCloseSocket;
	iCloseSocket = closesocket(TCPClientSocket);

	if (iCloseSocket == SOCKET_ERROR)
	{
		printf("Closing socket failed with error: %d", WSAGetLastError());
	}

	int iWsaCleanup;
	iWsaCleanup = WSACleanup();
	if (iWsaCleanup == SOCKET_ERROR)
	{
		printf("Cleanup socket failed with error: %d", WSAGetLastError());
	}

	return 0;

}

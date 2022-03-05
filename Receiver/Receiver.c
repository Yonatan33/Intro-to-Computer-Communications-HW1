/* Ofir Yaffe - , Yonatan Gartenberg - 311126205 */
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
#include <WinSock2.h>
#include "../Utils/Constants.h"


#pragma comment(lib, "Ws2_32.lib")
#define REMOTE_HOST_IP "127.0.0.1"
#define LOCAL_HOST_IP "127.0.0.1"
#define IN_PORT 6342 

int main(int argc, char* argv[])
{
	SOCKET TCPServerSocket;
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR)
	{
		printf("Error at WSAStartup()\n");
	}

	struct sockaddr_in TCPServerAddr;
	struct sockaddr_in TCPClientAddr;

	TCPServerAddr.sin_family = AF_INET;
	TCPServerAddr.sin_addr.s_addr = inet_addr(LOCAL_HOST_IP);
	TCPServerAddr.sin_port = htons(IN_PORT);

	TCPServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (TCPServerSocket == INVALID_SOCKET)
	{
		printf("Scoket creation failed with error: %d", WSAGetLastError());
	}

	int iBind;

	iBind = bind(TCPServerSocket, (SOCKADDR*)&TCPServerAddr, sizeof(TCPServerAddr));
	if (iBind == SOCKET_ERROR)
	{
		printf("Bind failed with error: %d", WSAGetLastError());
	}

	int iListen;

	iListen = listen(TCPServerSocket, 2);
	if (iListen == SOCKET_ERROR)
	{
		printf("Listen failed with error: %d", WSAGetLastError());
	}


	SOCKET acceptSocket;
	int iTCPClientAddr = sizeof(TCPClientAddr);
	acceptSocket = accept(TCPServerSocket, (SOCKADDR*)&TCPClientAddr, &iTCPClientAddr);
	if (acceptSocket == INVALID_SOCKET)
	{
		printf("Accept failed with error: %d", WSAGetLastError());
	}


	int iSend;
	char  sendBuf[512] = "Hello from Server!";
	int iSendBuf = strlen(sendBuf) + 1;
	iSend = send(acceptSocket, sendBuf, iSendBuf, 0);
	if (iSend == SOCKET_ERROR)
	{
		printf("Send failed with error: %d", WSAGetLastError());
	}

	int iReceive;
	char receiveBuf[512];
	int  iReceiveBuf = strlen(receiveBuf) + 1;
	iReceive = recv(acceptSocket, receiveBuf, iReceiveBuf, 0);
	if (iReceive == SOCKET_ERROR)
	{
		printf("Receive failed with error: %d", WSAGetLastError());
	}
	printf(receiveBuf);

	int iCloseSocket = closesocket(TCPServerSocket);
	if (iCloseSocket == SOCKET_ERROR)
	{
		printf("Closing socket failed with error: %d", WSAGetLastError());
	}


	int iWsaCleanup = WSACleanup();
	if (iWsaCleanup == SOCKET_ERROR)
	{
		printf("Cleanup failed with error: %d", WSAGetLastError());
	}
	

	return 0;

}

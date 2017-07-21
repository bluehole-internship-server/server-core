#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"mswsock.lib")

#include <winsock2.h>
#include <ws2tcpip.h>
#include <mswsock.h>
#include <stdio.h>
#include "ThreadPool.h"

#define SERVER_PORT 5150
#define IO_BUFF_SIZE 64

void PrintServerMessage(char * message)
{
	printf("SERVER::%s\n", message);
}

void PrintError(char * target, int errorCode)
{
	printf("SERVER::%s Failed with Error :%d\n", target, errorCode);
}


HANDLE CreateIocp(DWORD concurrent)
{
	return CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, concurrent);
}

BOOL ConnectIocp(HANDLE iocp, HANDLE device, DWORD completionKey)
{
	return CreateIoCompletionPort(device, iocp, completionKey, 0) == iocp;
}


struct IODATA
{
	OVERLAPPED overlapped;
	char buff[IO_BUFF_SIZE];
	WSABUF wsabuf;
	SOCKET targetSocket;

	IODATA();
};

IODATA::IODATA()
{
	SecureZeroMemory(this, sizeof(IODATA));
	wsabuf.len = IO_BUFF_SIZE;
	wsabuf.buf = buff;
}

int main()
{
	//----------------------------------------
	// Declare and initialize variables
	WSADATA wsaData;
	int iResult = 0;
	BOOL bRetVal = FALSE;

	HANDLE hCompPort;
	HANDLE hCompPort2;

	LPFN_ACCEPTEX lpfnAcceptEx = NULL;
	GUID GuidAcceptEx = WSAID_ACCEPTEX;
	WSAOVERLAPPED olOverlap;

	SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET AcceptSocket = INVALID_SOCKET;
	sockaddr_in service;
	char lpOutputBuf[1024];
	int outBufLen = 1024;
	DWORD dwBytes;

	hostent *thisHost;
	char *ip;
	u_short port;

	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR) {
		wprintf(L"Error at WSAStartup\n");
		return 1;
	}

	hCompPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, (u_long)0, 0);
	if (hCompPort == NULL) {
		wprintf(L"CreateIoCompletionPort failed with error: %u\n",
			GetLastError());
		WSACleanup();
		return 1;
	}

	ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ListenSocket == INVALID_SOCKET) {
		wprintf(L"Create of ListenSocket socket failed with error: %u\n",
			WSAGetLastError());
		WSACleanup();
		return 1;
	}

	CreateIoCompletionPort((HANDLE)ListenSocket, hCompPort, (u_long)0, 0);

	port = 5150;
	ZeroMemory(&service, sizeof(service));
	service.sin_family = AF_INET;
	service.sin_port = htons(port);
	service.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(ListenSocket, (SOCKADDR *)& service, sizeof(service)) == SOCKET_ERROR) {
		wprintf(L"bind failed with error: %u\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	iResult = listen(ListenSocket, 100);
	if (iResult == SOCKET_ERROR) {
		wprintf(L"listen failed with error: %u\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	iResult = WSAIoctl(ListenSocket, SIO_GET_EXTENSION_FUNCTION_POINTER,
		&GuidAcceptEx, sizeof(GuidAcceptEx),
		&lpfnAcceptEx, sizeof(lpfnAcceptEx),
		&dwBytes, NULL, NULL);
	if (iResult == SOCKET_ERROR) {
		wprintf(L"WSAIoctl failed with error: %u\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	// Empty our overlapped structure and accept connections.
	memset(&olOverlap, 0, sizeof(olOverlap));

	core::ThreadPool tp(16);
	auto input = [](LPVOID param) {
		HANDLE completionPort = (HANDLE)param;
		DWORD iocpRecieved;
		SOCKET iocpClientSocket;
		IODATA iocpIoData;

		while (TRUE)
		{
			GetQueuedCompletionStatus(completionPort, &iocpRecieved, (PULONG_PTR)&iocpClientSocket, (LPOVERLAPPED*)&iocpIoData, INFINITE);
			if (iocpRecieved == 0)
				PrintServerMessage("iocpRecieved == 0");

			iocpIoData.buff[iocpRecieved] = 0;
			PrintServerMessage(iocpIoData.buff);
			// WSASend
			DWORD flag{ 0 };
			if (WSARecv(iocpClientSocket, &(iocpIoData.wsabuf), 1, &iocpRecieved, &flag, &(iocpIoData.overlapped), NULL) == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)
				PrintError("WSARecv", WSAGetLastError());
		}
	};
	tp.Enqueue(input, hCompPort);

	while (TRUE)
	{
		SOCKET NewConnection = accept(ListenSocket, NULL, NULL);
		if (NewConnection == INVALID_SOCKET)
		{
		}
		else
		{
			puts("Client Á¢¼Ó");
			ConnectIocp(hCompPort, (HANDLE)NewConnection, (DWORD)NewConnection);
			IODATA clientIoData;
			DWORD recieved = 0, flag = 0;
			clientIoData.targetSocket = NewConnection;

			if (WSARecv(NewConnection, &(clientIoData.wsabuf), 1, &recieved, &flag, &(clientIoData.overlapped), NULL) == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)
				Sleep(1);
		}
	}


	// Continue on to use send, recv, TransmitFile(), etc.,.
	//...

	return 0;
}

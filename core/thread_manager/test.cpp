#pragma comment(lib,"ws2_32.lib")
#include <WinSock2.h>
#include <iostream>
#include <Windows.h>
#include "ThreadManager.h"

#define AMOUNT 8
#define SERVER_PORT 5150
#define IO_BUFF_SIZE 64

struct IODATA
{
    OVERLAPPED overlapped;
    char buff[IO_BUFF_SIZE];
    WSABUF wsabuf;
    SOCKET targetSocket;

    IODATA()
    {
        SecureZeroMemory(this, sizeof(IODATA));
        wsabuf.len = IO_BUFF_SIZE;
        wsabuf.buf = buff;
    }
};

BOOL AssociateIocp(HANDLE iocp, HANDLE device, DWORD key)
{
    return CreateIoCompletionPort(device, iocp, key, 0) == iocp;
}

VOID CALLBACK my_callback(
    PTP_CALLBACK_INSTANCE Instance,
    PVOID                 Parameter,
    PTP_WORK              Work);

int main()
{
    core::ThreadManager threadManager;
    HANDLE iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, AMOUNT);
    threadManager.AddWork(my_callback, iocp, 2, NULL);

    WSADATA wsaData;
    int err = WSAStartup(MAKEWORD(2, 2), &wsaData);
    SOCKADDR_IN ServerAddr;

    // 0.0.0.0::5150
    ServerAddr.sin_family = AF_INET;
    ServerAddr.sin_port = htons(SERVER_PORT);
    ServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    SOCKET ListeningSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
    bind(ListeningSocket, (SOCKADDR *)&ServerAddr, sizeof(ServerAddr));
    puts("bind is Completed.");

    listen(ListeningSocket, 5);
    puts("listening...");

    while (TRUE) {
        SOCKET NewConnection = accept(ListeningSocket, NULL, NULL);
        {
            puts("Client Á¢¼Ó");
            std::cout << AssociateIocp(iocp, (HANDLE)NewConnection, (DWORD)NewConnection);
            IODATA clientIoData;
            DWORD recieved = 0, flag = 0;
            clientIoData.targetSocket = NewConnection;

            WSARecv(NewConnection, &(clientIoData.wsabuf), 1, &recieved, &flag, &(clientIoData.overlapped), NULL);
        }
    }
    threadManager.JoinAll();

    return 0;
}

VOID CALLBACK my_callback(
    PTP_CALLBACK_INSTANCE Instance,
    PVOID                 Parameter,
    PTP_WORK              Work)
{
    HANDLE completionPort = (HANDLE)Parameter;
    DWORD iocpRecieved;
    SOCKET iocpClientSocket;
    IODATA iocpIoData;

    while (TRUE)
    {
        GetQueuedCompletionStatus(completionPort, &iocpRecieved, (PULONG_PTR)&iocpClientSocket, (LPOVERLAPPED*)&iocpIoData, INFINITE);
        if (iocpRecieved == 0)
            puts("iocpRecieved == 0");

        iocpIoData.buff[iocpRecieved] = 0;
        puts(iocpIoData.buff);
        // WSASend
        DWORD flag{ 0 };
        WSARecv(iocpClientSocket, &(iocpIoData.wsabuf), 1, &iocpRecieved, &flag, &(iocpIoData.overlapped), NULL);
    }
}
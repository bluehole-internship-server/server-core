#pragma once
#include <Windows.h>
#include <WinSock2.h>

enum IoType
{
	IO_NONE,
	IO_ACCEPT,
	IO_RECV,
	IO_CONNECT
};

struct IoContext
{
	IoContext();

	OVERLAPPED overlapped;
	SOCKET socket;
	WSABUF buffer;
	IoType io_type;
};
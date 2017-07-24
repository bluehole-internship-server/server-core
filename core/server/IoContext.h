#pragma once
#include <WinSock2.h>
#include "Client.h"

enum IoType
{
	IO_NONE,
	IO_ACCEPT,
	IO_RECV_READY,
	IO_RECV,
	IO_CONNECT,
	IO_DISCONNECT
};

struct IoContext
{
	IoContext();
	~IoContext();

	OVERLAPPED overlapped_;
	Client * client_;
	WSABUF buffer_;
	IoType io_type_;
};
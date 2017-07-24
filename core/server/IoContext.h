#pragma once
#include <WinSock2.h>

enum IoType
{
	IO_NONE,
	IO_ACCEPT,
	IO_RECV_READY,
	IO_RECV,
	IO_CONNECT,
	IO_DISCONNECT
};

class Client;
struct IoContext
{
	IoContext();
	IoContext(Client * client, IoType io_type);
	~IoContext();

	OVERLAPPED overlapped_;
	Client * client_;
	WSABUF buffer_;
	IoType io_type_;
};
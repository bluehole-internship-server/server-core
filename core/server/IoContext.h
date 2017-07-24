#pragma once
#include <WinSock2.h>
#include "Client.h"

#define SEND_BUFFER_SIZE 32
#define RECV_BUFFER_SIZE 256

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
	Client client_;
	WSABUF buffer_;
	IoType io_type_;
	char * send_buffer_;
	char * recv_buffer_;
};
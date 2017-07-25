#pragma once
#include <WinSock2.h>
#include <stdio.h>
#include "IoContext.h"

#define SEND_BUFFER_SIZE 32
#define RECV_BUFFER_SIZE 256

class Client
{
public:
	Client();
	~Client();
	BOOL PrepareReceive();
	BOOL Receive();
	BOOL PostReceive(DWORD);

	SOCKET socket_;
	char * nickname_;
	char send_buffer_[SEND_BUFFER_SIZE];
	char recv_buffer_[RECV_BUFFER_SIZE];
};


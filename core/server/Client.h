#pragma once
#include <Winsock2.h>

#define SEND_BUFFER_SIZE 32
#define RECV_BUFFER_SIZE 256

class Client
{
public:
	Client();
	~Client();

	SOCKET socket_;
	char * nickname_;
	char * send_buffer_;
	char * recv_buffer_;
};


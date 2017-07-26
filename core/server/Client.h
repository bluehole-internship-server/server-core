#pragma once
#include <WinSock2.h>
#include <stdio.h>
#include "IoContext.h"
#include "Buffer.h"

#define SEND_BUFFER_SIZE 32
#define RECV_BUFFER_SIZE 2560
namespace core
{
class Client
{
public:
	Client();
	~Client();
	BOOL PrepareReceive();
	BOOL Receive();
	BOOL PostReceive(DWORD);
	BOOL PostReceive(DWORD, std::function<void(IoContext *)>&, IoContext&);
	BOOL Send(char *, DWORD);
	BOOL PostSend(DWORD);
	BOOL Disconnect();

	SOCKET socket_;
	char * nickname_;
	Buffer send_buffer_;
	Buffer recv_buffer_;
};
}
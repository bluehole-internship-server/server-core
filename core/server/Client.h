#pragma once
#include <Winsock2.h>

class Client
{
public:
	Client(SOCKET &&);
	~Client();
	void SetNickname(char *);
	SOCKET GetSocket();

private:
	SOCKET socket_;
	char * nickname_;

};


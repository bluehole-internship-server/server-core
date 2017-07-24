#pragma once
#include <Winsock2.h>

struct Client
{
	SOCKET socket_;
	char * nickname_;
};


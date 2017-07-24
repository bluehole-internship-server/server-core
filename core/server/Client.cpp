#include "Client.h"
#include <utility>

Client::Client(SOCKET &&socket) : socket_(std::move(socket))
{
}

Client::~Client()
{
}

void Client::SetNickname(char * nickname)
{
	nickname_ = nickname;
}

SOCKET Client::GetSocket()
{
	return socket_;
}

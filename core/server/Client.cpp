#include "Client.h"

Client::Client()
{
	send_buffer_ = new char[SEND_BUFFER_SIZE];
	recv_buffer_ = new char[RECV_BUFFER_SIZE];
}

Client::~Client()
{
	delete send_buffer_;
	delete recv_buffer_;
}

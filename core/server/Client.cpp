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

BOOL Client::PrepareReiceve()
{
	wprintf(L"Accepted.\n");

	IoContext * recv_ready_context = new IoContext(this, IO_RECV_READY);
	DWORD recieved_bytes = 0;
	DWORD flags = 0;

	wprintf(L"Accpted Socket is %llu.\n", socket_);
	WSARecv(socket_, &recv_ready_context->buffer_, 1, &recieved_bytes, &flags, (LPWSAOVERLAPPED)recv_ready_context, NULL);
	return TRUE;
}

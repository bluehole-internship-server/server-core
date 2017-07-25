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

BOOL Client::PrepareReceive()
{
	IoContext * recv_ready_context = new IoContext(this, IO_RECV_READY);
	DWORD recieved_bytes = 0;
	DWORD flags = 0;

	WSARecv(socket_, &recv_ready_context->buffer_, 1, &recieved_bytes, &flags, (LPWSAOVERLAPPED)recv_ready_context, NULL);
	return TRUE;
}

BOOL Client::Receive()
{
	wprintf(L"Receive is Prepared.\n");
	
	IoContext * recv_context = new IoContext(this, IO_RECV);
	DWORD recieved_bytes = 0;
	DWORD flags = 0;
	recv_context->buffer_.len = RECV_BUFFER_SIZE;
	recv_context->buffer_.buf = recv_buffer_;

	if (WSARecv(socket_, &recv_context->buffer_, 1, &recieved_bytes, &flags, (LPWSAOVERLAPPED)recv_context, NULL) == SOCKET_ERROR)
		wprintf(L"BYE\n");

	return TRUE;
}

BOOL Client::PostReceive(DWORD received_bytes)
{
	wprintf(L"Received.\n");
	
	recv_buffer_[received_bytes] = 0;
	printf("Received Data : %s\n", recv_buffer_);
	return TRUE;
}

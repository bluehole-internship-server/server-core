#include "Client.h"
#include "Server.h"

namespace core
{
Client::Client()
{
}
Client::~Client()
{
	puts("Good Bye.");
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
		return FALSE;

	return TRUE;
}
BOOL Client::PostReceive(DWORD received_bytes)
{
	wprintf(L"Received.\n");

	recv_buffer_[received_bytes] = 0;
	printf("Received Data : %s\n", recv_buffer_);
	return TRUE;
}
BOOL Client::Send(char * data, DWORD size)
{
	IoContext * send_context = new IoContext(this, IO_SEND);
	memmove(send_buffer_, data, size);
	send_context->buffer_.buf = send_buffer_;
	send_context->buffer_.len = size;
	DWORD sent, flags;
	WSASend(socket_, &(send_context->buffer_), 1, &sent, 0, (LPWSAOVERLAPPED)send_context, nullptr);
	return 0;
}
BOOL Client::Disconnect()
{
	IoContext * disconnect_context = new IoContext(this, IO_DISCONNECT);
	core::Server::DisconnectEx(socket_, (LPWSAOVERLAPPED)disconnect_context, 0, 0);
	return 0;
}
}
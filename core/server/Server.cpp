#include "Server.h"

LPFN_DISCONNECTEX core::Server::DisconnectEx = nullptr;
LPFN_ACCEPTEX core::Server::AcceptEx = nullptr;
LPFN_CONNECTEX core::Server::ConnectEx = nullptr;
char core::Server::accept_buffer_[64] = { 0, };
core::ThreadPool * core::Server::thread_pool_ = nullptr;

namespace core 
{
Server::Server()
{
}


Server::~Server()
{
}
VOID Server::Init()
{
	// WinSock Init
	WSADATA wsa_data;
	_ASSERT(WSAStartup(MAKEWORD(2, 2), &wsa_data) == NO_ERROR);

	// Listen
	listen_socket_ = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	_ASSERT(listen_socket_ != INVALID_SOCKET);
	int opt = 1;
	setsockopt(listen_socket_, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(int));

	// Bind on 0.0.0.0::listen_port_
	SOCKADDR_IN server_addr;
	SecureZeroMemory(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(listen_port_);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	_ASSERT(bind(listen_socket_, (SOCKADDR *)&server_addr, sizeof(server_addr)) != SOCKET_ERROR);

	// Get Function Pointers
	DWORD bytes = 0; 
	GUID guidDisconnectEx = WSAID_DISCONNECTEX;
	_ASSERT(WSAIoctl(listen_socket_, SIO_GET_EXTENSION_FUNCTION_POINTER,
		&guidDisconnectEx, sizeof(GUID), &DisconnectEx, sizeof(LPFN_DISCONNECTEX), &bytes, NULL, NULL) != SOCKET_ERROR);

	GUID guidAcceptEx = WSAID_ACCEPTEX;
	_ASSERT(WSAIoctl(listen_socket_, SIO_GET_EXTENSION_FUNCTION_POINTER,
		&guidAcceptEx, sizeof(GUID), &AcceptEx, sizeof(LPFN_ACCEPTEX), &bytes, NULL, NULL) != SOCKET_ERROR);

	GUID guidConnectEx = WSAID_CONNECTEX;
	_ASSERT(WSAIoctl(listen_socket_, SIO_GET_EXTENSION_FUNCTION_POINTER,
		&guidConnectEx, sizeof(GUID), &ConnectEx, sizeof(LPFN_CONNECTEX), &bytes, NULL, NULL) != SOCKET_ERROR);

	// Create and Associate IOCP
	completion_port_ = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	_ASSERT(completion_port_ != NULL);
	HANDLE associated_port = CreateIoCompletionPort((HANDLE)listen_socket_, completion_port_, 0, 0);
	_ASSERT(completion_port_ == associated_port);
	 
	// Create New Thread Pool
	if (thread_pool_ == nullptr)
		thread_pool_ = new ThreadPool(32);
}
VOID Server::SetListenPort(USHORT port)
{
	listen_port_ = port;
}
VOID Server::Run()
{
	_ASSERT(listen(listen_socket_, SOMAXCONN) != SOCKET_ERROR);
	SOCKET client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	DWORD received_bytes;
	OVERLAPPED overlapped;
	SecureZeroMemory(&overlapped, sizeof(overlapped));
	while(TRUE)
		AcceptEx(listen_socket_, client_socket, accept_buffer_, 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &received_bytes, &overlapped);
}
void Server::PrintError(wchar_t * target, DWORD error_code)
{
	wprintf(L"%s�� �����߽��ϴ�. ���� �ڵ�: %d\n", target, error_code);
}
}
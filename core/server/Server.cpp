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
	int result = 0;

	// WinSock Init
	WSADATA wsa_data;
	result = WSAStartup(MAKEWORD(2, 2), &wsa_data);
	_ASSERT( result == NO_ERROR);

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
	result = bind(listen_socket_, (SOCKADDR *)&server_addr, sizeof(server_addr));
	_ASSERT( result != SOCKET_ERROR);

	// Get Function Pointers
	DWORD bytes = 0; 
	GUID guidDisconnectEx = WSAID_DISCONNECTEX;
	result = WSAIoctl(listen_socket_, SIO_GET_EXTENSION_FUNCTION_POINTER,
		&guidDisconnectEx, sizeof(GUID), &DisconnectEx, sizeof(LPFN_DISCONNECTEX), &bytes, NULL, NULL);
	_ASSERT( result != SOCKET_ERROR);

	GUID guidAcceptEx = WSAID_ACCEPTEX;
	result = WSAIoctl(listen_socket_, SIO_GET_EXTENSION_FUNCTION_POINTER,
		&guidAcceptEx, sizeof(GUID), &AcceptEx, sizeof(LPFN_ACCEPTEX), &bytes, NULL, NULL);
	_ASSERT( result != SOCKET_ERROR);

	GUID guidConnectEx = WSAID_CONNECTEX;
	result = WSAIoctl(listen_socket_, SIO_GET_EXTENSION_FUNCTION_POINTER,
		&guidConnectEx, sizeof(GUID), &ConnectEx, sizeof(LPFN_CONNECTEX), &bytes, NULL, NULL);
	_ASSERT( result != SOCKET_ERROR);

	// Create and Associate IOCP
	completion_port_ = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	_ASSERT(completion_port_ != NULL);
	HANDLE associated_port = CreateIoCompletionPort((HANDLE)listen_socket_, completion_port_, (ULONG)listen_socket_, 0);
	_ASSERT(completion_port_ == associated_port);
	 
	// Create New Thread Pool
	if (thread_pool_ == nullptr) {
		thread_pool_ = new ThreadPool(WORKER_AMOUNT);
		for (int i = 0; i < WORKER_AMOUNT; ++i)
			thread_pool_->Enqueue(IocpWork, *this);
	}
}
VOID Server::SetListenPort(USHORT port)
{
	listen_port_ = port;
}
VOID Server::IocpWork(Server &server)
{
	while (TRUE) {
		DWORD received_bytes = 0;
		IoContext * io_context = nullptr;
		ULONG_PTR key = 0;
		GetQueuedCompletionStatus(server.completion_port_, &received_bytes, (PULONG_PTR)&key, (LPOVERLAPPED *)&io_context, INFINITE);
		
		if (key == server.listen_socket_)
			wprintf(L"Key is ListenSocket.\n");
		else
			wprintf(L"Key is %llu.\n", key);
			
		switch (io_context->io_type_) {
			case IO_ACCEPT:
				wprintf(L"Accepted.\n");
				{
					IoContext * recv_ready_context = new IoContext();
					recv_ready_context->io_type_ = IO_RECV_READY;
					recv_ready_context->client_.socket_ = io_context->client_.socket_;

					DWORD recvbytes = 0;
					DWORD flags = 0;

					/// start async recv
					wprintf(L"Accpted Socket is %llu.\n", io_context->client_.socket_);
					WSARecv(io_context->client_.socket_, &recv_ready_context->buffer_, 1, &recvbytes, &flags, (LPWSAOVERLAPPED)recv_ready_context, NULL);
				}
				break;
			case IO_RECV_READY:
				wprintf(L"Receive is Prepared.\n");
				{
					IoContext * recv_context = new IoContext();
					recv_context->io_type_ = IO_RECV;
					recv_context->client_.socket_ = io_context->client_.socket_;
					
					DWORD recvbytes = 0;
					DWORD flags = 0;
					recv_context->buffer_.len = RECV_BUFFER_SIZE;
					recv_context->buffer_.buf = recv_context->client_.recv_buffer_;

					wprintf(L"Receive Prepared Socket is %llu.\n", io_context->client_.socket_);
					if(WSARecv(io_context->client_.socket_, &recv_context->buffer_, 1, &recvbytes, &flags, (LPWSAOVERLAPPED)recv_context, NULL) == SOCKET_ERROR)
						wprintf(L"BYE\n");
				}
				break;
			case IO_RECV:
				wprintf(L"Received.\n");
				{
					io_context->buffer_.buf[received_bytes] = 0;
					printf("Received Data : %s\n", io_context->buffer_.buf);
					IoContext * recv_ready_context = new IoContext();
					recv_ready_context->io_type_ = IO_RECV_READY;
					recv_ready_context->client_.socket_ = io_context->client_.socket_;

					DWORD recvbytes = 0;
					DWORD flags = 0;

					/// start async recv
					wprintf(L"Received Socket is %llu.\n", io_context->client_.socket_);
					WSARecv(io_context->client_.socket_, &recv_ready_context->buffer_, 1, &recvbytes, &flags, (LPWSAOVERLAPPED)recv_ready_context, NULL);
				}
				break;
			default:
				wprintf(L"What? %d\n", io_context->io_type_);
				break;
		}
	}
}
VOID Server::Run()
{
	int result = 0;
	result = listen(listen_socket_, SOMAXCONN);
	_ASSERT( result != SOCKET_ERROR);
	
	while (TRUE) {
		DWORD received_bytes;
		IoContext * io_context = new IoContext();
		io_context->io_type_ = IO_ACCEPT;
		io_context->client_.socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		SOCKET client_socket = io_context->client_.socket_;
		
		//wprintf(L"Socket is %llu.\n", client_socket);
		auto result = CreateIoCompletionPort((HANDLE)client_socket, completion_port_, (ULONG_PTR)client_socket, 0);
		_ASSERT(result == completion_port_);
				
		AcceptEx(listen_socket_, client_socket, accept_buffer_, 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &received_bytes, (LPOVERLAPPED)io_context);
		Sleep(1000);
	}
}
void Server::PrintError(wchar_t * target, DWORD error_code)
{
	wprintf(L"%s가 실패했습니다. 에러 코드: %d\n", target, error_code);
}
}
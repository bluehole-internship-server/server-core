#ifndef UNICODE
#define UNICODE
#endif

#pragma once
#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"mswsock.lib")

#include <winsock2.h>
#include <ws2tcpip.h>
#include <mswsock.h>
#include <stdio.h>
#include <functional>
#include "ThreadPool.h"

namespace core
{
class Server
{
public:
	Server();
	~Server();
	VOID Init();
	VOID SetListenPort(USHORT);
	template <class F, class... Args>
	auto AddWork(F&& f, Args&&... args)
		-> std::future<typename std::result_of<F(Args...)>::type>;
	VOID Run();

	static LPFN_DISCONNECTEX DisconnectEx;
	static LPFN_ACCEPTEX AcceptEx;
	static LPFN_CONNECTEX ConnectEx;
	static char accept_buffer_[64];

private:
	static ThreadPool *thread_pool_;
	HANDLE completion_port_;
	SOCKET listen_socket_;
	USHORT listen_port_;

	void PrintError(wchar_t * target, DWORD error_code);
};	
}

#include "Server.inl"
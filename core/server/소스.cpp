#include "Server.h"

int main()
{
	core::Server server;
	server.SetListenPort(5150);
	server.SetReceiveHandler([](core::IoContext * io_context) {
		io_context->client_->Send(io_context->client_->recv_buffer_, io_context->received_);
	});
	server.Init();
	server.Run();

	return 0;
}

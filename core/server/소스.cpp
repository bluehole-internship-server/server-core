#include "Server.h"

int main()
{
	core::Server server;
	server.SetListenPort(5150);
	server.SetReceiveHandler([](core::IoContext * io_context) {
		core::Client * client = io_context->client_;
		client->Send(client->recv_buffer_.Read(), io_context->received_);
	});
	server.Init();
	server.Run();

	return 0;
}

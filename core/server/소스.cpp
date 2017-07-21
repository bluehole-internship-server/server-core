#include "Server.h"

int main()
{
	core::Server server;
	server.SetListenPort(5150);
	server.Init();
	server.Run();

	return 0;
}

//
// server.cpp
//

#include "server.hpp"

namespace core::udp {
Server::Server(short port, int num_thread)
    : port_(port)
    , num_thread_(num_thread)
    , session_manager_(Endpoint(port, "127.0.0.1"), num_thread)
{
}

bool Server::Run()
{
    bool ret = session_manager_.Run();
    if (ret == false) return false;
    while (1);
    return ret;
}

bool Server::RunNonBlock()
{
    bool ret = session_manager_.Run();
    return ret;
}
}
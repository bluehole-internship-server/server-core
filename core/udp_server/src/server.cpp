//
// server.cpp
//

#include "server.hpp"

namespace core::udp {
Server::Server(short port, int num_io_thread, int num_task_thread)
    : port_(port)
    , session_manager_(Endpoint(port, "127.0.0.1"), num_io_thread, num_task_thread)
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
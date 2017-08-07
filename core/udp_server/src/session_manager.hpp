//
// session_manager.hpp
//
// session in udp can be defined with client's endpoint
//

#pragma once

#include <map>
#include <functional>
#include <thread>
#include <vector>

#include "session.hpp"
#include "socket.hpp"
#include "endpoint.hpp"

namespace core::udp {
class SessionManager {
public:
    SessionManager(Endpoint& endpoint, int num_thread);

    bool Run();

    inline void SetAcceptHandler(std::function<void(Session*)> &ahandler)
    {
        is_inited_all |= 0x01;
        ahandler_ = ahandler;
    }

    inline void SetPacketHandler(std::function<void(Session*, Packet&)>
        &phandler)
    {
        is_inited_all |= 0x10;
        phandler_ = phandler;
    }

private:
    HANDLE iocp_;
    void iocp_task();
    
    Socket socket_;
    
    std::vector<std::thread> threads_;
    int num_thread_;

    char is_inited_all;
    std::function<void(Session*)> ahandler_;
    std::function<void(Session*, Packet&)> phandler_;

    /* TODO */
    std::map<Endpoint, Session*> sessions_;
};
}
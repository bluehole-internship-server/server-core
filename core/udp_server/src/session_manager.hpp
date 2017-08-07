//
// session_manager.hpp
//
// session in udp can be defined with client's endpoint
//

#pragma once

#include <unordered_map>
#include <functional>

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
    Socket socket_;
    
    int num_thread_;
    char is_inited_all;

    std::function<void(Session*)> ahandler_;
    std::function<void(Session*, Packet&)> phandler_;

};
}
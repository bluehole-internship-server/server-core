//
// server.hpp
//

#pragma once

#include <functional>

#include "session_manager.hpp"
#include "session.hpp"

namespace core::udp {
class Server {
public:
    explicit Server(short port, int num_thread);
    ~Server() = default;
    
    bool Run();
    bool RunNonBlock();
    
    inline void SetAcceptHandler(std::function<void(Session*)>
        &ahandler)
    {
        session_manager_.SetAcceptHandler(ahandler);
    }

    inline void SetAcceptHandler(void (*ahandler)(Session*))
    {
        session_manager_.SetAcceptHandler(
            std::function<void(Session*)>(ahandler));
    }

    inline void SetPacketHandler(std::function<void(Session*, Packet&)>
        &phandler)
    {
        session_manager_.SetPacketHandler(phandler);
    }

    inline void SetPacketHandler(void(*phandler)(Session*, Packet&))
    {
        session_manager_.SetPacketHandler(
            std::function<void(Session*, Packet&)>(phandler));
    }

private:
    SessionManager session_manager_;

    short port_;
    short num_thread_;
};
}
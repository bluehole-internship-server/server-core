//
// session.hpp
//

#pragma once

#include "socket.hpp"

namespace core::udp {
class Session {
public:
    Session(Socket &manager_socket, Endpoint &endpoint);

    void* Data() { return data_; }
    void Send(Packet& packet);

private:
    void* data_;
    Socket &manager_socket_;
    Endpoint endpoint_;
};
}
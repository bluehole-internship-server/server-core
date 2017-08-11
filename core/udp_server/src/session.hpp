//
// session.hpp
//

#pragma once

#include "socket.hpp"

namespace core::udp {
class Session {
public:
    Session(Socket &manager_socket, Endpoint &endpoint)
        : manager_socket_(manager_socket)
        , endpoint_(endpoint)
    {

    }

    void* Data() { return data_; }
    void Send(Packet& packet)
    {
        manager_socket_.Send(packet, endpoint_, false);
    }

    Endpoint& GetEndpoint()
    {
        return endpoint_;
    }

private:
    void* data_;
    Socket &manager_socket_;
    Endpoint endpoint_;
};
}
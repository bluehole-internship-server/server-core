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
        , is_alive_(0)
        , data_(nullptr)
    {

    }

    void*& Data() { return data_; }
    void Send(Packet& packet)
    {
        manager_socket_.Send(packet, endpoint_, false);
    }

    Endpoint& GetEndpoint()
    {
        return endpoint_;
    }

private:
    friend class SessionManager;

    short is_alive_;

    void* data_;
    Socket &manager_socket_;
    Endpoint endpoint_;
};
}
//
// socket.hpp
//

#pragma once

#include <memory>

#include <windows.h>

#include "packet.hpp"
#include "endpoint.hpp"

namespace core::udp {
class Socket {
public:
    explicit Socket(Endpoint &endpoint);
    explicit Socket();

    void Send(Packet &packet, Endpoint &remote_endpoint);
    void Recv();

    std::unique_ptr<Endpoint>& GetRemoteEndpoint() { return remote_endpoint_; }
    std::unique_ptr<Endpoint>& GetEndpoint() { return endpoint_; }

private:
    SOCKET socket_;
    
    std::unique_ptr<Endpoint> endpoint_;
    std::unique_ptr<Endpoint> remote_endpoint_;
};
}

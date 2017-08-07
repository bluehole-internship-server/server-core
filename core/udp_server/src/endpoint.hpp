//
// endpoint.hpp
//

#pragma once

#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

namespace core::udp {
class Endpoint {
public:
    // IP has to point raw string literal
    Endpoint(short port, const char* IP)
        : port_(port)
        , IP_(IP)
    {
        addr_.sin_family = AF_INET;
        addr_.sin_port = htons(port_);
        InetPton(AF_INET, (TCHAR*)IP, &addr_.sin_addr);
    }

    inline short Port() { return port_; }
    inline const char* IP() { return IP_; }
    inline sockaddr_in& Addr() { return addr_; }

private:
    sockaddr_in addr_;

    short port_;
    const char* IP_;
};
}
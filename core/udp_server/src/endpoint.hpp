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
    Endpoint(unsigned short port, const char* IP)
        : port_(port)
        , IP_(IP)
    {
        memset(&addr_, 0, sizeof(addr_));
        addr_.sin_family = AF_INET;
        addr_.sin_port = htons(port_);
        InetPtonA(AF_INET, IP, &addr_.sin_addr);
    }

    Endpoint() { memset(&addr_, 0, sizeof(addr_)); }

    bool operator<(const Endpoint& rep) const
    {
        return this->addr_.sin_addr.s_addr < rep.addr_.sin_addr.s_addr ||
            ( this->addr_.sin_addr.s_addr == rep.addr_.sin_addr.s_addr &&
              this->addr_.sin_port < rep.addr_.sin_port );
    }

    bool operator==(const Endpoint& rep) const
    {
        return this->addr_.sin_addr.s_addr == rep.addr_.sin_addr.s_addr &&
            this->addr_.sin_port == rep.addr_.sin_port;
    }

    inline unsigned short Port() { return port_; }
    inline const char* IP() { return IP_; }
    inline sockaddr_in& Addr() { return addr_; }

private:
    sockaddr_in addr_;

    unsigned short port_;
    const char* IP_;
};
}
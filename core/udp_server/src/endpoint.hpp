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
        memset(&addr_, 0, sizeof(addr_));
        addr_.sin_family = AF_INET;
        addr_.sin_port = htons(port_);
        InetPtonA(AF_INET, IP, &addr_.sin_addr);
    }

    Endpoint() { memset(&addr_, 0, sizeof(addr_)); }

    bool operator<(const Endpoint& rep) const {
        /*
        long laddr = *(long*)&addr_.sin_addr;
        long raddr = *(long*)&addr_.sin_addr;
        short lport = addr_.sin_port;
        short rport = rep.addr_.sin_port;
        */
        return this < &rep;
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
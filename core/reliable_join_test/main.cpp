//
// main.cpp
//

#include <iostream>

#include "../udp_server/src/server.hpp"

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "../x64/Release/udp_server.lib")

bool init_wsa()
{
    WSAData wsa_data;
    return WSAStartup(MAKEWORD(2, 2), &wsa_data) == 0;
}

void packet_handler(core::udp::Session*, core::udp::Packet&) { } // ignore packet

// how about tls?
void accept_handler(core::udp::Session* session)
{
    std::cout << "Hello, world!\n";
}

struct packet
{
    short size;
    short type;
    short seq;
};
int main()
{
    init_wsa();

    core::udp::Server server(4000, 1);
    server.SetAcceptHandler(&accept_handler);
    server.SetPacketHandler(&packet_handler);

    server.RunNonBlock();

    SOCKET socket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    sockaddr_in server_addr;

    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(4000);
    InetPtonA(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    packet p;
    p.size = 2;
    p.type = -1;
    p.seq = 3;
    
    ::sendto(socket, (const char*)&p, 6, 0, (const sockaddr*)&server_addr, sizeof(server_addr));
    ::recv(socket, (char*)&p, sizeof(p), 0);
    if (p.type == -2) p.type = -3;
    ::sendto(socket, (const char*)&p, 6, 0, (const sockaddr*)&server_addr, sizeof(server_addr));
    std::cout << p.type << std::endl;
}
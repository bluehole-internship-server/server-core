//
// socket.hpp
//

#pragma once

#include <memory>
#include <queue>
#include <mutex>

#include <windows.h>

#include "packet.hpp"
#include "endpoint.hpp"

namespace core::udp {
class Socket {
private:
    friend class SessionManager;
    const static int socket_buf_size = 512;
    struct io_data {
        io_data()
        {
            wsa_buf.buf = buffer;
            wsa_buf.len = socket_buf_size;
        }

        OVERLAPPED overlapped;
        WSABUF wsa_buf;
        char buffer[socket_buf_size];
    };

    struct send_request {
        send_request(Packet &packet_, Endpoint &endpoint_)
            : packet(packet_)
            , endpoint(endpoint_)
        {
        }

        send_request(send_request &&send_req)
            : packet(std::move(send_req.packet))
            , endpoint(std::move(send_req.endpoint))
        {
        }

        send_request(send_request &send_req) = default;

        Packet packet;
        Endpoint endpoint;
    };

public:
    explicit Socket(Endpoint &endpoint);
    explicit Socket(Endpoint &endpoint, HANDLE iocp);

    explicit Socket();
    explicit Socket(HANDLE iocp);

    void Send(Packet &packet, Endpoint &remote_endpoint, bool immediately);
    void Recv();

    const Endpoint& GetRemoteEndpoint() { return remote_endpoint_; }
    const Endpoint& GetEndpoint() { return endpoint_; }

private:
    bool is_with_iocp_;

    io_data read_io_data_;
    io_data write_io_data_;

    SOCKET socket_;
    HANDLE iocp_;

    Endpoint endpoint_;
    Endpoint remote_endpoint_;

    int from_len;

    /* TODO : change this to mt queue, ptr vs obj */
    std::queue<send_request> send_req_queue_;
    std::mutex mtx_send_req_queue_;
};
}

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
    const static int socket_buf_size = 512;
    struct io_data {
        enum Op {
            READ,
            WRITE
        };

        io_data()
        {
            read_wsa_buf.buf = read_buffer;
            write_wsa_buf.buf = write_buffer;
            read_wsa_buf.len = write_wsa_buf.len = socket_buf_size;
        }

        OVERLAPPED read_overlapped;
        OVERLAPPED write_overlapped;
        WSABUF read_wsa_buf;
        WSABUF write_wsa_buf;
        char write_buffer[socket_buf_size];
        char read_buffer[socket_buf_size];

        Op op;
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

    io_data io_data_;
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

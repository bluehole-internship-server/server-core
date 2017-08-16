//
// socket.hpp
//

#pragma once

#include <memory>
#include <queue>
#include <mutex>

#include <windows.h>

#include "memory_pool.hpp"

#include "packet.hpp"
#include "endpoint.hpp"

namespace core::udp {
class Socket {
private:
    friend class SessionManager;
    const static int socket_buf_size = 512;
    
    struct io_data {
        OVERLAPPED overlapped;
        WSABUF wsa_buf;
        bool is_read_io_data;
    };

    struct read_io_data : io_data {
        read_io_data()
        {
            wsa_buf.buf = buffer;
            wsa_buf.len = socket_buf_size;
            from_len = sizeof(remote_endpoint.Addr());
            is_read_io_data = true;
            memset(&overlapped, 0, sizeof(overlapped));
        }
        Endpoint remote_endpoint;
        int from_len;
        char buffer[socket_buf_size];
    };

    struct write_io_data : io_data {
        write_io_data(Packet &packet)
        {
            data_ = packet.data_;
            wsa_buf.buf = (char*)packet.data_.get();
            wsa_buf.len = (unsigned short)packet.data_->size + 4;
            is_read_io_data = false;
            memset(&overlapped, 0, sizeof(overlapped));
        }
        std::shared_ptr<Packet::data> data_;
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

    const Endpoint& GetEndpoint() { return endpoint_; }

private:
    bool is_with_iocp_;

    SOCKET socket_;
    HANDLE iocp_;

    Endpoint endpoint_;

    unsigned long received_bytes;

    static core::ObjectPool<Socket::read_io_data> r_io_data_pool_;
    static core::ObjectPool<Socket::write_io_data> w_io_data_pool_;
};
}

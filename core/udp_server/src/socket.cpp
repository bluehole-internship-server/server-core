//
// socket.cpp
//

#include "socket.hpp"

namespace core::udp {
Socket::Socket(Endpoint &endpoint)
    : endpoint_(endpoint)
    , iocp_(nullptr)
    , is_with_iocp_(false)
    , from_len(sizeof(remote_endpoint_.Addr()))
{
    socket_ = ::socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    ::bind(socket_, (const sockaddr*)&endpoint_.Addr(),
        sizeof(endpoint_.Addr()));
}

Socket::Socket(Endpoint &endpoint, HANDLE iocp)
    : endpoint_(endpoint)
    , iocp_(iocp)
    , from_len(sizeof(remote_endpoint_.Addr()))
{
    socket_ = ::socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    ::bind(socket_, (const sockaddr*)&endpoint_.Addr(),
        sizeof(endpoint_.Addr()));
    if (iocp_ != nullptr &&
        CreateIoCompletionPort((HANDLE)socket_, iocp_, (ULONG_PTR)this, 0)) {
        is_with_iocp_ = true;
    }
}

Socket::Socket()
    : iocp_(nullptr)
    , is_with_iocp_(false)
    , from_len(sizeof(remote_endpoint_.Addr()))
{
    socket_ = ::socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
}

Socket::Socket(HANDLE iocp)
    : from_len(sizeof(remote_endpoint_.Addr()))
{
    socket_ = ::socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (iocp_ != nullptr &&
        CreateIoCompletionPort((HANDLE)socket_, iocp_, (ULONG_PTR)this, 0)) {
        is_with_iocp_ = true;
    }
}

void Socket::Recv()
{
    /* TODO */
    if (!is_with_iocp_) return;

    unsigned long flags = 0;

    WSARecvFrom(socket_, &(read_io_data_.wsa_buf), 1, &received_bytes, &flags,
        (sockaddr*)&remote_endpoint_.Addr(), &from_len,
        &(read_io_data_.overlapped), NULL);
}

void Socket::Send(Packet &packet, Endpoint &remote_endpoint, bool immediately)
{
    unsigned long flags = 0;
    Packet* ptr_packet;
    Endpoint* ptr_endpoint;

    if (immediately == false) {
        mtx_send_req_queue_.lock();
        /* TODO : packet has to be preserved */
        send_req_queue_.emplace(packet, remote_endpoint);
        if (send_req_queue_.size() > 1) {
            mtx_send_req_queue_.unlock();
            return;
        }
        ptr_packet = &send_req_queue_.front().packet;
        ptr_endpoint = &send_req_queue_.front().endpoint;
        mtx_send_req_queue_.unlock();
    } else {
        ptr_packet = &packet;
        ptr_endpoint = &remote_endpoint;
    }
    write_io_data_.wsa_buf.len = packet.Size() + 4;
    WSASendTo(socket_, &(write_io_data_.wsa_buf), 1, nullptr, flags,
        (sockaddr*)&ptr_endpoint->Addr(), sizeof(ptr_endpoint->Addr()),
        &(write_io_data_.overlapped), nullptr);
}
}
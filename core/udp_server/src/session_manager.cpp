//
// session_manager.cpp
//

#include "session_manager.hpp"

namespace core::udp {
SessionManager::SessionManager(Endpoint& endpoint, int num_thread)
    : num_thread_(num_thread)
    , is_inited_all(0x00)
{
    iocp_ = CreateIoCompletionPort(INVALID_HANDLE_VALUE,
        NULL, NULL, num_thread_);
    if (!iocp_) {
        is_inited_all = 0x100;
    }
    else {
        socket_ = std::make_unique<Socket>(endpoint, iocp_);
    }
}

bool SessionManager::Run()
{
    if (is_inited_all != 0x11) return false;
    for (int i = 0; i < num_thread_; ++i) {
        threads_.emplace_back([this]() { 
            this->socket_->Recv();
            this->iocp_task();
        });
    }
    return true;
}


void SessionManager::iocp_task()
{
    while (1) {
        Socket* socket = nullptr;
        Socket::read_io_data* io_data = nullptr;
        unsigned long bytes;
        bool succ = GetQueuedCompletionStatus(iocp_, &bytes,
            (PULONG_PTR)&socket, (LPOVERLAPPED*)&io_data, INFINITE);

        if (!succ || succ == 0) continue;
        if (bytes < 4) continue;

        if (io_data == &socket->read_io_data_) {
            short size;
            short type;
            size = *reinterpret_cast<short*>(io_data->buffer);
            type = *reinterpret_cast<short*>(io_data->buffer + 2);

            if (bytes == 6 && size == 2 && type == -1) { // SYN
                Session* new_session = new Session(*socket_,
                    io_data->remote_endpoint);
                pending_sessions_.insert({new_session->GetEndpoint(), new_session});
                // TODO : delete existing session
                new_session->Send(Packet(2, -2, io_data->buffer + 4));
            }
            else if (bytes == 6 && size == 2 && type == -3) { // ACK
                // Lock
                auto it = pending_sessions_.find(io_data->remote_endpoint);
                if (it == pending_sessions_.end()) continue;

                Session* pending_session = it->second;

                pending_sessions_.erase(it);
                sessions_.insert({pending_session->GetEndpoint(), pending_session});
                // Unlock
    
                ahandler_(pending_session);
            }
            else if (size + 4 == bytes) {
                std::map<Endpoint, Session*>::iterator it;
                it = sessions_.find(io_data->remote_endpoint);
                if (it == sessions_.end()) continue;
                phandler_(it->second, Packet(io_data->buffer));
            }

            socket_->Recv();
        } else {
            Socket::write_io_data* ptr = (Socket::write_io_data*)io_data;
            delete ptr;
        }
    }
}

}
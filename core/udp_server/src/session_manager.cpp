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
        Socket::io_data* io_data = nullptr;
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
            if (bytes == 4 && size == 0 && type == -1) {
                Session* new_session = new Session(*socket_,
                    socket->remote_endpoint_);
                sessions_.insert({socket->remote_endpoint_, new_session});
                ahandler_(new_session);
                
            } else if(size + 4 == bytes) {
                std::map<Endpoint, Session*>::iterator it;
                it = sessions_.find(socket->remote_endpoint_);
                if (it == sessions_.end()) continue;
                phandler_(it->second, Packet(io_data->buffer));
            }
            socket_->Recv();
        } else if (io_data == &socket->write_io_data_) {
            socket->mtx_send_req_queue_.lock();
            std::queue<Socket::send_request>* queue =
                &socket_->send_req_queue_;
            if (queue->size() == 0) {
                // can't be happen but..
                socket->mtx_send_req_queue_.unlock();
                return;
            }
            socket->send_req_queue_.pop();
            if (queue->size() != 0) {
                Socket::send_request req = queue->front();
                socket->Send(req.packet, req.endpoint, true);
            }
            socket->mtx_send_req_queue_.unlock();
            socket_->Recv();
        }
    }
}

}
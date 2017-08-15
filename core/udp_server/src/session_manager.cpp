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
        is_inited_all = 0x1000;
    }
    else {
        socket_ = std::make_unique<Socket>(endpoint, iocp_);
    }
    have_to_stop_ = false;
}

bool SessionManager::Run()
{
    if (is_inited_all != 0x0111) return false;
    for (int i = 0; i < num_thread_; ++i) {
        threads_.emplace_back([this]() { 
            this->socket_->Recv();
            this->iocp_task();
        });
    }

    // check heartbeat
    threads_.emplace_back([this]() {
        std::vector<Session*> disconnected;
        while (true) {
            disconnected.clear();
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            mtx_sessions_.lock();
            for (auto session = sessions_.begin(); session != sessions_.end();) {
                InterlockedDecrement16(&session->second->is_alive_);
                if (session->second->is_alive_ < -1) {
                    disconnected.emplace_back(session->second);
                    session = sessions_.erase(session);
                    continue;
                }
                session++;
            }
            mtx_sessions_.unlock();

            for (auto &session : disconnected) {
                dhandler_(session);
            }
        }
    });
    return true;
}


void SessionManager::iocp_task()
{
    while (!have_to_stop_) {
        Socket* socket = nullptr;
        Socket::io_data* io_data = nullptr;
        unsigned long bytes;
        bool succ = GetQueuedCompletionStatus(iocp_, &bytes,
            (PULONG_PTR)&socket, (LPOVERLAPPED*)&io_data, wait_time);

        if (!succ) continue;
        if (bytes < 4) continue;

        if (io_data->is_read_io_data) {
            Socket::read_io_data* read_io_data = reinterpret_cast<Socket::read_io_data*>(io_data);
            short size;
            short type;
            size = *reinterpret_cast<short*>(read_io_data->buffer);
            type = *reinterpret_cast<short*>(read_io_data->buffer + 2);

            if (bytes == 6 && size == 2 && type == -1) { // SYN
                Session* new_session = new Session(*socket_,
                    read_io_data->remote_endpoint);
                
                // Lock
                mtx_pending_sessions_.lock();
                pending_sessions_.insert({new_session->GetEndpoint(), new_session});
                mtx_pending_sessions_.unlock();
                // Unlock

                new_session->Send(Packet(2, -2, read_io_data->buffer + 4));
            }
            else if (bytes == 6 && size == 2 && type == -3) { // ACK
                // Lock          
                mtx_pending_sessions_.lock();
                auto it = pending_sessions_.find(read_io_data->remote_endpoint);
                if (it == pending_sessions_.end()) {
                    mtx_pending_sessions_.unlock();
                    continue;
                }
                Session* pending_session = it->second;

                pending_sessions_.erase(it);
                mtx_sessions_.lock();
                sessions_.insert({pending_session->GetEndpoint(), pending_session});
                mtx_sessions_.unlock();
                mtx_pending_sessions_.unlock();
                // Unlock
                
                std::thread([&]() {ahandler_(pending_session); }).detach();
            }
            else if (bytes == 4 && size == 0 && type == -1) { // heart beat
                mtx_sessions_.lock();
                auto it = sessions_.find(read_io_data->remote_endpoint);
                if (it == sessions_.end()) {
                    mtx_sessions_.unlock();
                    continue;
                }
                mtx_sessions_.unlock();
                InterlockedIncrement16(&it->second->is_alive_);
            }
            else if (size + 4 == bytes) {
                std::map<Endpoint, Session*>::iterator it;
                // Lock
                mtx_sessions_.lock();
                it = sessions_.find(read_io_data->remote_endpoint);
                if (it == sessions_.end()) {
                    mtx_sessions_.unlock();
                    continue;
                }
                mtx_sessions_.unlock();
                // Unlock
                std::thread([&]() {phandler_(it->second, Packet(read_io_data->buffer)); }).detach();
            }
            delete read_io_data;
            socket_->Recv();
        } else {
            Socket::write_io_data* ptr = reinterpret_cast<Socket::write_io_data*>(io_data);
            delete ptr;
        }
    }
}

}
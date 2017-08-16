//
// session_manager.cpp
//

#include "session_manager.hpp"

namespace core::udp {

core::ObjectPool<Session> SessionManager::session_pool_;

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
            std::this_thread::sleep_for(std::chrono::milliseconds(10000));
            lock_sessions_.Lock();
            for (auto session = sessions_.begin(); session != sessions_.end();) {
                InterlockedDecrement16(&session->second->is_alive_);
                if (session->second->is_alive_ < -1) {
                    disconnected.emplace_back(session->second);
                    session = sessions_.erase(session);
                    continue;
                }
                session++;
            }
            lock_sessions_.Unlock();

            for (auto session = disconnected.begin(); session != disconnected.end();) {
                if (InterlockedDecrement16(&(*session)->is_alive_) == -3) {
                    dhandler_((*session));
                    delete (*session);
                    session = disconnected.erase(session);
                    continue;
                }
                session++;
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
                Session* new_session = new(session_pool_.Malloc()) Session(*socket_,
                    read_io_data->remote_endpoint);
                
                // Lock
                lock_pending_sessions_.Lock();
                pending_sessions_.insert({new_session->GetEndpoint(), new_session});
                lock_pending_sessions_.Unlock();
                // Unlock

                new_session->Send(Packet(2, -2, read_io_data->buffer + 4));
            }
            else if (bytes == 6 && size == 2 && type == -3) { // ACK
                // Lock          
                lock_pending_sessions_.ReadLock();
                auto it = pending_sessions_.find(read_io_data->remote_endpoint);
                if (it == pending_sessions_.end()) {
                    lock_pending_sessions_.ReadUnlock();
                    continue;
                }
                Session* pending_session = it->second;
                lock_pending_sessions_.ReadUnlock();

                lock_pending_sessions_.Lock();
                pending_sessions_.erase(it);
                lock_pending_sessions_.Unlock();

                lock_sessions_.Lock();
                sessions_.insert({pending_session->GetEndpoint(), pending_session});
                lock_sessions_.Unlock();
                // Unlock
                
                std::thread([&]() {ahandler_(pending_session); }).detach();
            }
            else if (bytes == 4 && size == 0 && type == -1) { // heart beat
                lock_sessions_.ReadLock();
                auto it = sessions_.find(read_io_data->remote_endpoint);
                if (it == sessions_.end()) {
                    lock_sessions_.ReadUnlock();
                    continue;
                }
                Session* session = it->second;
                lock_sessions_.ReadUnlock();
                InterlockedIncrement16(&session->is_alive_);
            }
            else if (size + 4 == bytes) {
                std::map<Endpoint, Session*>::iterator it;
                // Lock
                lock_sessions_.ReadLock();
                it = sessions_.find(read_io_data->remote_endpoint);
                if (it == sessions_.end()) {
                    lock_sessions_.ReadUnlock();
                    continue;
                }
                Session* session = it->second;
                lock_sessions_.ReadUnlock();
                // Unlock

                // have to be safe
                
                if (InterlockedIncrement16(&session->is_alive_) > -2) {
                    std::thread([&]() 
                    {
                        phandler_(session, Packet(read_io_data->buffer));
                    }).detach();
                }
            }
            Socket::r_io_data_pool_.Free(read_io_data);
            socket_->Recv();
        } else {
            Socket::write_io_data* ptr = reinterpret_cast<Socket::write_io_data*>(io_data);
            Socket::w_io_data_pool_.Free(ptr);
        }
    }
}

}
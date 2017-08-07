//
// session_manager.cpp
//

#include "session_manager.hpp"

namespace core::udp {
SessionManager::SessionManager(Endpoint& endpoint, int num_thread)
    : socket_(endpoint)
    , num_thread_(num_thread)
    , is_inited_all(0x00)
{
}

bool SessionManager::Run()
{
    if (is_inited_all != 0x11) return false;
    /* TODO */
    return true;
}

}
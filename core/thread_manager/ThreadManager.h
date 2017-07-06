#pragma once
#include <Windows.h>
#include <unordered_set>
#include "Iocp.h"

namespace core 
{
class ThreadManager
{
public:
    ThreadManager();
    ~ThreadManager();
    
    BOOL set_maximum_thread_count(int);
    int get_usable_thread_count(int);

    BOOL add_iocp(Iocp iocp);
    BOOL remove_iocp(Iocp iocp);
    BOOL pause_iocp(Iocp iocp);
private:
    int maximum_thread_count = 0;
    int current_thread_count = 0;
    std::unordered_set<Iocp> iocp_set;
};
}
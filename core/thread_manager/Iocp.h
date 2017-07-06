#pragma once
#include <Windows.h>
#include <vector>
#include "ThreadManager.h"

namespace core
{
class Iocp
{
public:
    Iocp();
    ~Iocp();
    static BOOL set_thread_manager(ThreadManager *);
    BOOL create(int number_of_concurrent_thread, void(*function));
    BOOL associate(HANDLE device, DWORD key);
    BOOL expand(int number_of_concurrent_thread);

private:
    static ThreadManager * thread_manager;
    HANDLE iocp;
    std::vector<HANDLE> thread_pool;
    int reserved_pool_size;

    BOOL run_threads(void(*function));
};

ThreadManager * Iocp::thread_manager = nullptr;

}
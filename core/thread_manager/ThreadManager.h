#pragma once
#include <Windows.h>

namespace core 
{
    class ThreadManager
    {
    public:
        ThreadManager();
        ~ThreadManager();
    private:
        int current_thread_count;
    };
}
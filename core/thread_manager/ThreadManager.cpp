#include "ThreadManager.h"

namespace core
{

ThreadManager::ThreadManager()
{
    GetSystemInfo(&system_info);
    maximum_thread_count = system_info.dwNumberOfProcessors * 2 + 2;
}

ThreadManager::~ThreadManager()
{
}

BOOL ThreadManager::set_maximum_thread_count(int)
{
    return 0;
}

int ThreadManager::get_usable_thread_count()
{
    return maximum_thread_count - current_thread_count;
}

ThreadManager::Iocp ThreadManager::create_iocp(char * name, int concurrent, unsigned (*function)(LPVOID))
{
    Iocp new_iocp;
    int real_thread_pool_size = concurrent * 2;
    if (current_thread_count + real_thread_pool_size > maximum_thread_count)
        new_iocp.iocp = NULL;
    else
    {
        current_thread_count += real_thread_pool_size;
        new_iocp.iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, concurrent);
        new_iocp.tp.resize(real_thread_pool_size);
        for (HANDLE th : new_iocp.tp)
            th = (HANDLE)_beginthreadex(NULL, 0, (_beginthreadex_proc_type)function, new_iocp.iocp, 0, NULL);
        iocps.insert({ name, new_iocp });
    }
    return new_iocp;
}

BOOL ThreadManager::associate_iocp(char * name, HANDLE device, DWORD key)
{
    return CreateIoCompletionPort(device, iocps[name].iocp, key, 0) == iocps[name].iocp;
}

BOOL ThreadManager::remove_iocp(char * name)
{
    return iocps.erase(name) == 1;
}

BOOL ThreadManager::pause_iocp(char * name)
{
    return 0;
}

BOOL ThreadManager::resume_iocp(char * name)
{
    return 0;
}

}
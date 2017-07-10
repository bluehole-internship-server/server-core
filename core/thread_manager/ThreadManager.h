#pragma once
#include <Windows.h>
#include <process.h>
#include <unordered_map>
#include <vector>

namespace core 
{
class ThreadManager
{
public:
    typedef struct _Iocp
    {
        HANDLE iocp;
        std::vector<HANDLE> tp;
    }Iocp;

    ThreadManager();
    ~ThreadManager();
    
    BOOL set_maximum_thread_count(int);
    int get_usable_thread_count();

    ThreadManager::Iocp create_iocp(char * name, int concurrent, unsigned (*function)(LPVOID));
    BOOL associate_iocp(char * name, HANDLE device, DWORD key);
    BOOL remove_iocp(char * name);
    BOOL pause_iocp(char * name);
    BOOL resume_iocp(char * name);

private:
    int maximum_thread_count = 0;
    int current_thread_count = 0;
    std::unordered_map<char *, Iocp> iocps;
    SYSTEM_INFO system_info;
};
}
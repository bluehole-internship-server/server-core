#pragma once
#include <Windows.h>
#include <unordered_map>

namespace core 
{
class ThreadManager
{
public:
    ThreadManager();
    ~ThreadManager();
    
    VOID SetMinimumWorker(DWORD size);
    VOID SetMaximumWorker(DWORD size);
    BOOL AddEnvironment(PCHAR name);
    BOOL AddWork(PTP_WORK_CALLBACK work, PVOID parameter, DWORD amount, PCHAR environment_name);
    BOOL JoinAll();

private:
    SYSTEM_INFO systemInfo_;
    PTP_POOL threadpool_;
    std::unordered_map<PCHAR, TP_CALLBACK_ENVIRON> callbackEnvironments_;
    TP_CALLBACK_ENVIRON defaultCallbackEnvironment_;
};
}
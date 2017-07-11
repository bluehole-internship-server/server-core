#pragma once
#include <Windows.h>
#include <process.h>
#include <unordered_map>
#include <vector>
#include <queue>
#include <functional>

namespace core 
{
class ThreadManager
{
public:
    ThreadManager();
    ~ThreadManager();
    
    BOOL AddEnvironment(PCHAR name);
    BOOL AddWork(PTP_WORK_CALLBACK work, PVOID parameter, PCHAR environment_name);
    BOOL JoinAll();

private:
    SYSTEM_INFO systemInfo_;
    PTP_POOL threadpool_;
    std::unordered_map<PCHAR, TP_CALLBACK_ENVIRON> callbackEnvironments_;
    TP_CALLBACK_ENVIRON defaultCallbackEnvironment_;
};
}
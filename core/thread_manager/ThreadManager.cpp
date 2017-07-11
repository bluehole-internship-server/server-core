#include "ThreadManager.h"

namespace core
{
ThreadManager::ThreadManager()
{
    threadpool_ = CreateThreadpool(NULL);

    GetSystemInfo(&systemInfo_);
    SetThreadpoolThreadMaximum(threadpool_, systemInfo_.dwNumberOfProcessors * 2);
    BOOL success = SetThreadpoolThreadMinimum(threadpool_, 1);
    _ASSERT(success);

    InitializeThreadpoolEnvironment(&defaultCallbackEnvironment_);
    // do Move
    defaultCallbackEnvironment_.CleanupGroup = CreateThreadpoolCleanupGroup();
    SetThreadpoolCallbackPool(&defaultCallbackEnvironment_, threadpool_);
}
ThreadManager::~ThreadManager()
{
}
VOID ThreadManager::SetMinimumWorker(DWORD size)
{
    BOOL result = SetThreadpoolThreadMinimum(threadpool_, size);
    _ASSERT(result);
}
VOID ThreadManager::SetMaximumWorker(DWORD size)
{
    SetThreadpoolThreadMaximum(threadpool_, size);
}
BOOL ThreadManager::AddEnvironment(PCHAR name)
{
    return TRUE;
}
BOOL ThreadManager::AddWork(PTP_WORK_CALLBACK work, PVOID parameter, DWORD amount, PCHAR environment_name)
{    
    BOOL hasName = FALSE;

    if (environment_name != NULL) {
        _ASSERT(callbackEnvironments_.find(environment_name) != callbackEnvironments_.end());
        hasName = TRUE;
    }

    for (DWORD i = 0; i < amount; ++i) {
        PTP_WORK newWork;
        if (hasName)
            newWork = CreateThreadpoolWork(work, parameter, &(callbackEnvironments_[environment_name]));
        else
            newWork = CreateThreadpoolWork(work, parameter, &defaultCallbackEnvironment_);
        _ASSERT(newWork != NULL);
        
        SubmitThreadpoolWork(newWork);
    }

    return TRUE;
}
BOOL ThreadManager::JoinAll()
{
    CloseThreadpoolCleanupGroupMembers(defaultCallbackEnvironment_.CleanupGroup, FALSE, NULL);
    for (auto environment : callbackEnvironments_) {
        CloseThreadpoolCleanupGroupMembers((environment.second.CleanupGroup), FALSE, NULL);
    }
    return 0;
}
}
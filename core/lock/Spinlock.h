#pragma once
#include <atomic>

namespace core 
{
class Spinlock
{
public:
	Spinlock();
    ~Spinlock();

	void Lock();
	void Unlock();
	void ReadLock();
	void ReadUnlock();

private:
	std::atomic<long> mLockFlag;
	std::atomic<long> reader;
};
}

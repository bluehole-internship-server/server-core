#include "Spinlock.h"
#include <Windows.h>

namespace core
{
Spinlock::Spinlock()
{
}

Spinlock::~Spinlock()
{
}

void Spinlock::Lock()
{
	for (;;)
	{
		if (mLockFlag.exchange(1) == 0)
		{
			while (reader.load() != 0) {}
			return;
		}

		Sleep(1);
	}
}

void Spinlock::Unlock()
{
	mLockFlag.exchange(0);

}

void Spinlock::ReadLock()
{
	for (;;)
	{
		if (!mLockFlag) {
			++reader;
			return;
		}
		Sleep(1);
	}
}

void Spinlock::ReadUnlock()
{
	--reader;
}

}
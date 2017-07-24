//
// types.hpp
//
// define types and configurations
//

#pragma once

#include <mutex>
#include <cstddef>

#include <Windows.h>

#include "Spinlock.h"

#define FORCE_INLINE __forceinline
#define THREAD_LOCAL _declspec(thread)

namespace core::memory {
    static const int MaxSize = 256 * 1024;
    static const int ClassSizesMax = 96; // because of cache line
    static const int PageShift = 13;
    static const int PageSize = 1 << PageShift;
    static const int Alignment = 8;
    static const int MaxPages = 1 << (20 - PageShift);
    static const int MinSystemAlloc = 128;
    static const int MaxListLength = 8192;

    typedef core::Spinlock Lock;
    typedef core::Spinlock SpinLock;

    typedef DWORD pthread_t;
    typedef unsigned long long page_id_t;

    static inline FORCE_INLINE void* & next_of(void* const ptr)
    {
        return *(static_cast<void **>(ptr));
    }
}
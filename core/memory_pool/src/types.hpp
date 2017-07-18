//
// types.hpp
//

#pragma once

#include <mutex>
#include <cstddef>

#include <Windows.h>

#define FORCE_INLINE __forceinline
#define THREAD_LOCAL _declspec(thread)

namespace core::memory {
    static const std::size_t MaxSize = 256 * 1024;
    static const std::size_t ClassSizesMax = 96; // because of cache line
    static const std::size_t PageShift = 13;
    static const std::size_t PageSize = 1 << PageShift;
    static const std::size_t Alignment = 8;
    static const std::size_t MaxPages = 1 << (20 - PageShift);

    typedef std::mutex Lock;
    typedef DWORD pthread_t;

    static inline FORCE_INLINE void* & next_of(void* const ptr)
    {
        return *(static_cast<void **>(ptr));
    }
}
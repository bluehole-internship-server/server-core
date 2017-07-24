//
// memory_pool.hpp
//

#pragma once

#include <cstddef>

#ifdef MALLOC_EXPORTS
#define MALLOC_API __declspec(dllexport)
#else
#define MALLOC_API __declspec(dllimport)
#endif

namespace core::memory {
MALLOC_API void* Malloc(size_t size);
MALLOC_API void  Free(void* ptr, size_t size);
}
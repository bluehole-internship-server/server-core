//
// memory_pool.hpp
//

#pragma once

#include <iostream>

#include <cstddef>

namespace core::memory {
void* Malloc(size_t size);
void Free(void* ptr);
}
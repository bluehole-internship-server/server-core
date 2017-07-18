//
// memory_pool.hpp
//

#pragma once

#include <iostream>

#include <cstddef>

namespace core::memory {
void* Malloc(std::size_t size);
void Free(void* ptr);
}
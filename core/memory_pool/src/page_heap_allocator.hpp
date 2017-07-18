//
// page_heap_allocator.hpp
//

#pragma once

namespace core::memory {

template<typename T>
class PageHeapAllocator {
public:
    T* New() { return nullptr; }
};
}
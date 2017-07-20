//
// statics.cpp
//

#pragma once

#include <assert.h>
#include <iostream>

#include "statics.hpp"
#include "malloc.hpp"

using namespace core::memory;

namespace core::memory {

PageHeap Static::page_heap_;
SpinLock Static::page_heap_lock_;
SizeMap Static::size_map_;
CentralFreeList Static::central_cache_[ClassSizesMax];
PageHeapAllocator<Span> Static::span_allocator_;

bool Static::is_inited_;

void Static::InitStaticVars() {
    size_map_.Init();
    span_allocator_.Init();
    page_heap_.Init();

    // TODO : Is it safe?
    for (int i = 0; i < size_map_.num_classes; i++) {
        central_cache_[i].Init(i);
    }

    is_inited_ = true;
}

void* sys_alloc(size_t size, size_t* actual_size, size_t alignment)
{
    const int page_size = get_page_size();

    // alignment shouldn't be larger than page size
    alignment = (std::max)((int)alignment, page_size);

    // get least upper bound
    size = ((size + alignment - 1) / alignment) * alignment;
    if (actual_size) *actual_size = size;

    void* p = VirtualAlloc(0, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

    assert((reinterpret_cast<uintptr_t>(p) & (alignment - 1)) == 0);

    return p;
}

int get_page_size()
{
    static int page_size = 0;
    if (page_size == 0) {
        SYSTEM_INFO system_info;
        GetSystemInfo(&system_info);
        page_size = (std::max)(system_info.dwPageSize,
            system_info.dwAllocationGranularity);
    }
    return page_size;
}
}

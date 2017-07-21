//
// statics.hpp
//

#pragma once

#include "types.hpp"
#include "size_map.hpp"
#include "central_freelist.hpp"
#include "page_heap_allocator.hpp"
#include "page_map.hpp"
#include "page_heap.hpp"

namespace core::memory {
class Static {
public:
    Static()
    {
        if (is_inited_ == false) {
            InitStaticVars();
        }
    }

    static void InitStaticVars();

    static PageHeap& page_heap() { return page_heap_; }
    static Lock& page_heap_lock() { return page_heap_lock_; }
    static SizeMap& size_map() { return size_map_; }
    static CentralFreeList* central_cache() { return central_cache_; }
    static PageHeapAllocator<Span>& span_allocator()
    {
        return span_allocator_;
    }

    static bool IsInited() { return is_inited_; }

private:
    static PageHeapAllocator<Span> span_allocator_;
    static PageHeap page_heap_;
    static SpinLock page_heap_lock_;
    static SizeMap size_map_;
    static CentralFreeList central_cache_[ClassSizesMax];
    static bool is_inited_;
};

int get_page_size();
void* sys_alloc(size_t size, size_t* actual_size, size_t alignment);

}
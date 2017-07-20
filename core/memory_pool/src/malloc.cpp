//
// malloc.cpp
//

#include <cassert>

#include "malloc.hpp"
#include "statics.hpp"
#include "thread_cache.hpp"

namespace core::memory {

void* do_large_malloc(size_t size)
{
    {
        Static::page_heap_lock().lock();
        // HACK
        Span* span = Static::page_heap().New(((size + PageSize - 1) / PageSize));
        void* ret = reinterpret_cast<void*>(((span->start) << PageShift));
        return ret;
        Static::page_heap_lock().unlock();
    }
}

void* do_malloc(size_t size)
{
    ThreadCache* cache = ThreadCache::GetCache();
    int c_idx = -1;
    size_t allocated_size = 0;

    if (!Static::size_map().SizeToClass(size, &c_idx)) {
        return do_large_malloc(size);
    }
    allocated_size = Static::size_map().ClassToSize(c_idx);
    return cache->Allocate(allocated_size, c_idx);
}

void* Malloc(size_t size)
{
    assert(Static::IsInited() == true);

    return do_malloc(size);
}

void Free(void* ptr)
{

}

static Static to_initialize;
}
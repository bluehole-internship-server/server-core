//
// thread_cache.cpp
//

#include "thread_cache.hpp"

namespace core::memory {

PageHeapAllocator<ThreadCache> threadcache_allocator;
ThreadCache* ThreadCache::thread_heaps_ = nullptr;
int ThreadCache::thread_heap_count_ = 0;

void ThreadCache::Init(pthread_t tid)
{
    // TODO : Not implemented
}

void* ThreadCache::fetch_from_central_cache(int c_idx, int byte_size)
{
    free_list& list = list_[c_idx];
    const int batch_size = Static::size_map().NumObjectsToMove(c_idx);
    const int num_to_move = (std::min)(list.max_length(), batch_size);
    
    // TODO : Need to implement after finishing central cache

    return nullptr;
}

ThreadCache* ThreadCache::create_cache()
{
    ThreadCache* heap = threadcache_allocator.New();
    pthread_t tid = GetCurrentThreadId();
    heap->Init(tid);
    heap->next_ = thread_heaps_;
    heap->prev_ = nullptr;

    if (thread_heaps_ != nullptr) thread_heaps_->prev_ = heap;
    // next memory steal?

    thread_heaps_ = heap;
    thread_heap_count_++;

    return heap;
}
}
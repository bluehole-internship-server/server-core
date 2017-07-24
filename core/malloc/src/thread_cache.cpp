//
// thread_cache.cpp
//

#include "thread_cache.hpp"

namespace core::memory {

PageHeapAllocator<ThreadCache> thread_cache_allocator;
ThreadCache* ThreadCache::thread_heaps_ = nullptr;

int ThreadCache::thread_heap_count_ = 0;
bool ThreadCache::is_inited_ = false;

struct THREAD_LOCAL ThreadCache::thread_local_data
    ThreadCache::thread_local_data_;

void ThreadCache::Init(pthread_t tid)
{
    next_ = nullptr;
    prev_ = nullptr;
    tid_ = tid;

    for (int c_idx = 0; c_idx < Static::size_map().num_classes; c_idx++) {
        list_[c_idx].init(Static::size_map().ClassToSize(c_idx));
    }
}

void* ThreadCache::fetch_from_central_cache(int byte_size, int c_idx)
{
    free_list& list = list_[c_idx];
    const int batch_size = Static::size_map().NumObjectsToMove(c_idx);
   
    //const int num_to_move = (std::min)(list.max_length(), batch_size);
    const int num_to_move = batch_size;

    void* start = nullptr;
    void* end;

    int count =
        Static::central_cache()[c_idx].Remove(num_to_move, &start, &end);
    if (count > 0) {
        count--; // return start immediately
        list.push_range(count, next_of(start), end);
    }

    return start;
}

ThreadCache* ThreadCache::create_cache()
{
    ThreadCache* heap = thread_cache_allocator.New();
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

void ThreadCache::init_module()
{
    thread_cache_allocator.Init();
    is_inited_ = true;
}
}
//
// thread_cache.hpp
//

#pragma once

#include <cassert>

#include "types.hpp"
#include "statics.hpp"
#include "page_heap_allocator.hpp"

namespace core::memory {
class ThreadCache {
private:
    class free_list {
    public:
        void init(size_t size)
        {
            list_ = NULL;
            length_ = 0;
            lowater_ = 0;
            max_length_ = 1;
            length_overages_ = 0;
            size_ = size;
        }

        int push(void* ptr)
        {
            next_of(ptr) = list_;
            list_ = ptr;
            length_++;
            return length_;
        }

        void* pop()
        {
            if (length_ == 0) return nullptr;
            length_--;
            if (length_ < lowater_) lowater_ = length_;
            void* ret = list_;
            list_ = next_of(list_);
            return ret;
        }

        void push_range(int n, void* start, void* end)
        {
            next_of(end) = list_;
            list_ = start;
            length_ += n;
        }

        void pop_range(int n, void** start, void** end)
        {
            if (n == 0 || n > length_) {
                *start = nullptr;
                *end = nullptr;
            }

            *start = *end = list_;
            for (int i = 0; i < n - 1; i++) {
                *end = next_of(*end);
            }

            list_ = next_of(*end);
            next_of(*end) = nullptr;
            length_ -= n;
            if (length_ < lowater_) lowater_ = length_;
        }

        int length() const { return length_; }
        int lowwatermark() const { return lowater_; }
        void clear_lowwatermark() { lowater_ = length_; }
        int max_length() const { return max_length_; }
        int length_overages() const { return length_overages_; }
        void set_length_overages(int new_count) {
            length_overages_ = new_count;
        }
        int object_size() const { return size_; }
        bool empty() const { return list_ == nullptr; }

    private:
        void* list_;
        int length_;
        int lowater_;
        int max_length_;
        int length_overages_;
        int size_;
    };

    struct thread_local_data {
        ThreadCache* fast_path_heap;
        ThreadCache* heap;
    };

public:
    static ThreadCache* GetCache();

    void Init(pthread_t tid);
    void* Allocate(size_t size, int c_idx);
    void Deallocate(void* ptr, int c_idx);

    ThreadCache* next_;
    ThreadCache* prev_;

private:
    void* fetch_from_central_cache(int byte_size, int c_idx);

    static ThreadCache* create_cache();

    static void init_module();

    static ThreadCache* thread_heaps_;
    static int thread_heap_count_;
    static bool is_inited_;

    free_list list_[ClassSizesMax];
    pthread_t tid_;
    
    static THREAD_LOCAL thread_local_data thread_local_data_;    
};

extern PageHeapAllocator<ThreadCache> thread_cache_allocator;

inline FORCE_INLINE ThreadCache* ThreadCache::GetCache()
{
    if (!is_inited_) {
        init_module();
    }
    ThreadCache* cache = thread_local_data_.heap;
    if (cache == nullptr)
        cache = thread_local_data_.heap = create_cache();

    return cache;
}

inline FORCE_INLINE void* ThreadCache::Allocate(size_t size, int c_idx)
{
    assert(Static::size_map().ClassToSize(c_idx) == size);

    if (!list_[c_idx].empty()) {
        void* ptr = list_[c_idx].pop();
        return ptr;
    }

    return fetch_from_central_cache(size, c_idx);
}

inline FORCE_INLINE void ThreadCache::Deallocate(void* ptr, int c_idx)
{
    // some corner case?
    list_[c_idx].push(ptr);
}
}
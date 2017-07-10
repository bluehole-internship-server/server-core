//
// pool.hpp
//

#pragma once

#include <cstdlib>

#include <algorithm>

#include "segregated_storage.hpp"

namespace core {

class singleton_pool;

class Pool {
public:
    explicit Pool(const std::size_t requested_size)
        : next_size_(32)
        , alloc_size_(std::max(requested_size, sizeof(void*)))
        , storage_(std::max(requested_size, sizeof(void*)))
    {
    }

    ~Pool()
    {
    }

    virtual void* Malloc() 
    {
        if (!Empty()) {
            return storage_.malloc();
        }
        return resize_and_malloc();
    }

    bool ReleaseMemory()
    {
        // TODO
    }

    inline bool PurgeMemory()
    {
        new (&storage_) segregated_storage(alloc_size_);
        return true;
    }

    virtual inline void Free(void* const chunk)
    {
        storage_.free(chunk);
    }

    inline bool Empty()
    {
        return storage_.empty();
    }

private:
    segregated_storage storage_;

    std::size_t next_size_;
    std::size_t alloc_size_;

    void* resize_and_malloc()
    {
        std::size_t total_size = next_size_ * alloc_size_;

        char* ptr = (char*)malloc(total_size);
        if (ptr == 0) return 0;

        next_size_ <<= 1;

        storage_.add_block(ptr, total_size, alloc_size_);

        return storage_.malloc();
    }
};
}


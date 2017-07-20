//
// page_heap_allocator.hpp
//

#pragma once

#include "types.hpp"
#include "statics.hpp"

namespace core::memory {

template<typename T>
class PageHeapAllocator {
public:
    void Init()
    {
        in_use_ = 0;
        free_area_ = nullptr;
        free_avail_ = 0;
        free_list_ = nullptr;

        // reserve some space at the begining
        Delete(New());
    }

    T* New()
    {
        void* ret;
        if (free_list_ != NULL) {
            ret = free_list_;
            free_list_ = next_of(free_list_);
        } else {
            if (free_avail_ < sizeof(T)) {
                // need more room
                meta_data_alloc();
            }
            ret = free_area_;
            free_area_ += sizeof(T);
            free_avail_ -= sizeof(T);
        }
        in_use_++;
        return reinterpret_cast<T*>(ret);
    }

    void Delete(T* ptr)
    {
        next_of(ptr) = free_list_;
        free_list_ = ptr;
        in_use_--;
    }

    int in_use() { return in_use_; }

private:
    static const int alloc_increment = 128 << 10;
    static const int meta_data_alignment = 8;

    bool meta_data_alloc();

    char* free_area_;
    size_t free_avail_;

    void* free_list_;

    int in_use_;
};

template<typename T>
bool PageHeapAllocator<T>::meta_data_alloc()
{
    free_avail_ = alloc_increment;
    free_area_ = reinterpret_cast<char*>(
        sys_alloc(alloc_increment, &free_avail_, meta_data_alignment));

    return free_area_ != nullptr;
}
}
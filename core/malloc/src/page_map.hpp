//
// page_map.hpp
//

#pragma once

#include "types.hpp"
#include "page_heap_allocator.hpp"

namespace core::memory {
// 2 level radix tree for 48 bits of address space
class PageMap {
private:
    static const int bits = 48 - PageShift;

    static const int leaf_bits = (bits + 1) / 2;
    static const int leaf_length = 1 << leaf_bits;

    static const int root_bits = bits - leaf_bits;
    static const int root_length = 1 << root_bits;

    struct Leaf {
        void* values[leaf_length];
    };

public:

    void Init()
    {
        leaf_allocator_.Init();
        memset(&root_, 0, sizeof(root_));
    }


    void* Get(uintptr_t n)
    {
        uintptr_t idx_1, idx_2;
        idx_1 = n >> leaf_bits;
        idx_2 = n & (leaf_length - 1);

        if ((n >> bits) > 0 || root_[idx_1] == nullptr) return nullptr;
        return root_[idx_1]->values[idx_2];
    }

    void Set(uintptr_t n, void* ptr)
    {
        uintptr_t idx_1, idx_2;
        idx_1 = n >> leaf_bits;
        idx_2 = n & (leaf_length - 1);
        if (root_[idx_1] == nullptr) {
            root_[idx_1] = leaf_allocator_.New();
            memset(root_[idx_1], 0, sizeof(Leaf));
        }
        root_[idx_1]->values[idx_2] = ptr;
    }

private:

    Leaf* root_[root_length];

    static PageHeapAllocator<Leaf> leaf_allocator_;
};

}
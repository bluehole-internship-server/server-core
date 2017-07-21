//
// page_map.hpp
//

#pragma once

#include "types.hpp"
#include "page_heap_allocator.hpp"

namespace core::memory {
// 3 level radix tree for 48 bits of address space
class PageMap {
private:
    static const int bits = 48;

    static const int interior_bits = (bits + 2) / 3;
    static const int interior_length = 1 << interior_bits;

    static const int leaf_bits = bits - 2 * interior_bits;
    static const int leaf_length = 1 << leaf_bits;

    struct Leaf {
        void* values[leaf_length];
    };

public:
    struct Node {
        Node* ptrs[interior_length];
    };

    void Init()
    {
        node_allocator_.Init();
        memset(&root_, 0, sizeof(root_));
    }


    void* Get(uintptr_t n)
    {
        uintptr_t idx_1, idx_2, idx_3;
        get_idices(n, idx_1, idx_2, idx_3);

        if ((n >> bits) > 0 || root_.ptrs[idx_1] == nullptr ||
            root_.ptrs[idx_1]->ptrs[idx_2] == nullptr) {
            return nullptr;
        }
        return reinterpret_cast<Leaf*>(root_.ptrs[idx_1]->ptrs[idx_2])->
            values[idx_3];
    }

    void Set(uintptr_t n, void* ptr)
    {
        uintptr_t idx_1, idx_2, idx_3;
        get_idices(n, idx_1, idx_2, idx_3);
        
        if (root_.ptrs[idx_1] == nullptr) {
            root_.ptrs[idx_1] = new_node();
        }
        
        if (root_.ptrs[idx_1]->ptrs[idx_2] == nullptr) {
            root_.ptrs[idx_1]->ptrs[idx_2] = new_node();
        }

        reinterpret_cast<Leaf*>(root_.ptrs[idx_1]->ptrs[idx_2])->values[idx_3] = ptr;
    }

private:
    Node* new_node()
    {
        Node* result = node_allocator_.New();
        if (result != nullptr) {
            memset(result, 0, sizeof(Node));
        }
        return result;
    }

    static inline void get_idices(uintptr_t n, uintptr_t &idx_1,
        uintptr_t &idx_2, uintptr_t &idx_3)
    {
        idx_1 = n >> (leaf_bits + interior_bits);
        idx_2 = (n >> leaf_bits) & (interior_length - 1);
        idx_3 = n & (leaf_length - 1);
    }

    Node root_;

    static PageHeapAllocator<Node> node_allocator_;
};

}
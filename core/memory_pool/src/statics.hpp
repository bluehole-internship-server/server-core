//
// statics.hpp
//

#pragma once

#include "types.hpp"
#include "size_map.hpp"
#include "central_freelist.hpp"

namespace core::memory {
class Static {
public:
    Static()
    {
        if (is_inited_ == false) {
            InitStaticVars();
            is_inited_ = true;
        }
    }

    static void InitStaticVars();

    static Lock& pageheap_lock() { return pageheap_lock_; }
    static SizeMap& size_map() { return size_map_; }
    static CentralFreeList& central_freelist() { return central_freelist_; }

    static bool IsInited() { return is_inited_; }

private:
    static Lock pageheap_lock_;
    static SizeMap size_map_;
    static CentralFreeList central_freelist_;
    static bool is_inited_;
};




}
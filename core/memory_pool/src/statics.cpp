//
// statics.cpp
//

#pragma once

#include "statics.hpp"

using namespace core::memory;

namespace core::memory {

Lock Static::pageheap_lock_;
SizeMap Static::size_map_;
CentralFreeList Static::central_freelist_;

bool Static::is_inited_;

void Static::InitStaticVars() {
    size_map_.Init();
}
}

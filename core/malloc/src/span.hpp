//
// span.hpp
//

#pragma once

#include "types.hpp"

namespace core::memory {
struct Span {
    page_id_t       start;
    Span*           next;
    Span*           prev;
    int             length;
    void*           objects;
    unsigned int    ref_count : 16;
    unsigned int    size_class : 8;
    unsigned int    location : 2;
    unsigned int    sample : 1;

    enum {
        IN_USE,
        ON_NORMAL_FREELIST,
        ON_RETURNED_FREELIST
    };
};
}
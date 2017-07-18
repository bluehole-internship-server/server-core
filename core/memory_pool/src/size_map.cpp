//
// size_map.cpp
//

#include <algorithm>

#include "size_map.hpp"

namespace core::memory {

// we only use this function for small sizes ( <= MaxSize )
static inline int FORCE_INLINE lg_floor(std::size_t n)
{
    int log = 0;
    int shift = 32;
    std::size_t x;
    while ((shift/=2) > 0) {
        x = (n >> shift);
        if (x != 0) {
            n = x;
            log += shift;
        }
    }
    return log;
}

int SizeMap::alignment_for_size(std::size_t size)
{
    // alignment has to be multiple of Alignment
    int ret = Alignment;
    if (size > MaxSize) {
        ret = PageSize;
    } else if (size >= 128) {
        ret = (1 << (lg_floor(size) - 3));
    } else if (size >= 16) {
        ret = 16;
    }
    return (std::min)(ret, (int)PageSize);
}

void SizeMap::Init()
{
    int c_idx = 1;
    int space = Alignment;

    // compute properties for each size class
    for (std::size_t size = Alignment; size <= MaxSize; size += space) {
        space = alignment_for_size(size);

        // why divided by four? 
        int blocks_to_move = num_move_objs(size) / 4;
        std::size_t page_size = 0;
        std::size_t num_pages = 0;

        do {
            page_size += PageSize;
            while ((page_size % size) > (page_size >> 3))
                page_size += PageSize;
        } while ((page_size / size) < blocks_to_move);

        num_pages = page_size >> PageShift;
        if (c_idx > 1 && num_pages == class_to_pages_[c_idx - 1]) {
            // See if we can merge this and prev one
            // num_pages << PageShift == page_size
            if ((page_size / size) ==
                ((class_to_pages_[c_idx - 1] << PageShift) /
                class_to_size_[c_idx - 1])) {
                class_to_size_[c_idx - 1] = (int)size;
                continue;
            }
        }

        class_to_pages_[c_idx] = (int)num_pages;
        class_to_size_[c_idx] = (int)size;
        c_idx++;
    }

    num_classes = c_idx;

    std::size_t next_size = 0;
    for (c_idx = 1; c_idx < num_classes; ++c_idx) {
        for (std::size_t size = next_size; size <= class_to_size_[c_idx];
            size += Alignment) {
            index_to_class_[size_to_index(size)] = c_idx;
        }
        next_size = class_to_size_[c_idx] + Alignment;
    }

    num_objects_to_move_[0] = 0;

    for (c_idx = 1; c_idx < num_classes; ++c_idx) {
        // TODO : why we don't divide with four?
        num_objects_to_move_[c_idx] = num_move_objs(ClassToSize(c_idx));
    }
}
}
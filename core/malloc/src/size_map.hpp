//
// size_map.hpp
//

#pragma once

#include <algorithm>

#include "types.hpp"

namespace core::memory {
class SizeMap {
public:
    void Init();
    
    inline int ClassToSize(int c_idx)
    {
        return class_to_size_[c_idx];
    }

    inline int ClassToPages(int c_idx)
    {
        return class_to_pages_[c_idx];
    }
    
    inline int NumObjectsToMove(int c_idx)
    {
        return num_objects_to_move_[c_idx];
    }

    inline bool SizeToClass(size_t size, int* c_idx)
    {
        int idx = -1;
        if ((idx = size_to_index(size)) == -1) return false;
        *c_idx = index_to_class_[idx];
        return true;
    }

    size_t num_classes;

private:
    static const size_t max_small_size = 1024;
    static const size_t index_array_size =
        ((MaxSize + 127 + 15360) >> 7) + 1;

    static inline int FORCE_INLINE size_to_index(size_t size)
    {
        if (size < 0 || size > MaxSize) return -1;
        if (size <= max_small_size) {
            return ((int)size + 7) >> 3;
        } else if (size <= MaxSize) {
            return ((int)size + 127 + 15360) >> 7;
        }
        return -1;
    }

    static inline FORCE_INLINE int num_move_objs(size_t size)
    {
        return (std::min)((std::max)(64 * 1024 / (int)size, 2), 32);
    }

    int alignment_for_size(size_t size);

    int class_to_size_[ClassSizesMax];
    int class_to_pages_[ClassSizesMax];
    int num_objects_to_move_[ClassSizesMax];

    unsigned char index_to_class_[index_array_size];

    // same with tc_malloc's thresholds

};
}
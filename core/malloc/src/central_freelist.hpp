//
// central_freelist.hpp
//

#pragma once

#include "types.hpp"
#include "span.hpp"

namespace core::memory {
class CentralFreeList {
private:
    struct TCEntry {
        void* head;
        void* tail;
    };

public:
    void Init(int c_idx);
    int Remove(int n, void** start, void** end);
    void Insert(int n, void* start, void* end);

private:
    int fetch_from_one_spans(int n, void** start, void** end);
    int fetch_from_one_spans_safe(int n, void** start, void** end);

    void release_lists_to_spans(void* start);
    void release_to_spans(void* start);

    void populate();
  
    static const int max_num_tc_entries = 64;

    Lock lock_;

    int size_class_;
    int num_spans_;
    int counter_;

    Span empty_;
    Span non_empty_;

    TCEntry tc_slots_[max_num_tc_entries];

    int used_slots_;
    int cache_size_;
    int max_cache_size_;
};
}
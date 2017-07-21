//
// page_heap.hpp
//

#pragma once

#include "types.hpp"
#include "span.hpp"
#include "page_map.hpp"

namespace core::memory {
class PageHeap {
private:
    struct SpanList {
        Span normal;
        Span returned;
    };

public:
    void Init();

    Span* New(int n);
    void Delete(Span* span);

    Span* GetDiscriptor(void* ptr);

    void RegisterSizeClass(Span* span, int c_idx);
    void SetCachedSizeClass(page_id_t page_id, int c_idx);

private:
    Span* new_span(page_id_t page_id, int len);
    Span* search_free_and_large_lists(int n);
    Span* search_large_lists(int n);
    Span* carve(Span* span, int n);

    void record_span(Span* span);
    void insert_to_free(Span* span);
    void remove_from_free(Span* span);

    bool grow_heap(int n);

    // free spans of length >= MaxPages
    SpanList large_;
    SpanList free_[MaxPages];

    PageMap page_map_;
};
}
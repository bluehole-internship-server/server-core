//
// page_heap.cpp
//

#include <algorithm>
#include <cassert>

#include "page_heap.hpp"
#include "statics.hpp"

namespace core::memory {
PageHeapAllocator<PageMap::Node> PageMap::node_allocator_;

void PageHeap::Init()
{
    page_map_.Init();

    large_.normal.next = &large_.normal;
    large_.normal.prev = &large_.normal;

    large_.returned.next = &large_.returned;
    large_.returned.prev = &large_.returned;

    for (int i = 0; i < MaxPages; i++) {
        free_[i].normal.next = &free_[i].normal;
        free_[i].normal.prev = &free_[i].normal;

        free_[i].returned.next = &free_[i].returned;
        free_[i].returned.prev = &free_[i].returned;
    }
}

Span* PageHeap::New(int n)
{
    Span* result = search_free_and_large_lists(n);
    if (result != nullptr) return result;
    if (grow_heap(n)) {
        result = search_free_and_large_lists(n);
    }
    return result;
}

void PageHeap::Delete(Span* span)
{
    // HACK
    insert_to_free(span);
}

Span* PageHeap::GetDiscriptor(void* ptr)
{
    return reinterpret_cast<Span*>(page_map_.Get((uintptr_t)ptr));
}

void PageHeap::RegisterSizeClass(Span* span, int c_idx)
{

}

void PageHeap::SetCachedSizeClass(page_id_t page_id, int c_idx)
{

}

Span* PageHeap::search_free_and_large_lists(int n)
{
    for (int s = n; s < MaxPages; s++) {
        Span* list = &free_[s].normal;
        if (list->next != list) {
            return carve(list->next, n);
        }
    }
    return search_large_lists(n);
}

Span* PageHeap::search_large_lists(int n)
{
    Span* best = nullptr;

    for (Span* span = large_.normal.next; span != &large_.normal;
        span = span->next) {
        if (span->length >= n) {
            if ((best == nullptr) || (span->length < best->length) ||
                (span->length == best->length && span->start < best->start)) {
                best = span;
            }
        }
    }

    if (best == nullptr) return nullptr;
    return carve(best, n);
}

Span* PageHeap::carve(Span* span, int n)
{
    int old_location = span->location;
    remove_from_free(span);
    span->location = Span::IN_USE;

    const int remain = span->length - n;
    if (remain > 0) {
        Span* left = new_span(span->start + n, remain);
        left->location = old_location;
        insert_to_free(left);

        span->length = n;
    }

    return span;
}

Span* PageHeap::new_span(page_id_t page_id, int len)
{
    Span* span = Static::span_allocator().New();
    memset(span, 0, sizeof(Span));
    span->start = page_id;
    span->length = len;
    record_span(span);

    return span;
}

void PageHeap::record_span(Span* span)
{
    page_map_.Set((uintptr_t)(span->start << PageShift), span);
}

void PageHeap::insert_to_free(Span* span)
{
    SpanList* list = (span->length < MaxPages) ?
        &free_[span->length] : &large_;

    span->next = list->normal.next;
    span->prev = &list->normal;

    list->normal.next->prev = span;
    list->normal.next = span;
}

void PageHeap::remove_from_free(Span* span)
{
    SpanList* list = (span->length < MaxPages) ?
        &free_[span->length] : &large_;

    span->prev->next = span->next;
    span->next->prev = span->prev;
    span->prev = nullptr;
    span->next = nullptr;
}

bool PageHeap::grow_heap(int n)
{
    int ask = (n < MinSystemAlloc) ? MinSystemAlloc : n;
    void* ptr = nullptr;
    size_t actual_size;

    ptr = sys_alloc(ask << PageShift, &actual_size, PageSize);
    
    ask = actual_size >> PageShift;

    page_id_t page_id = reinterpret_cast<uintptr_t>(ptr) >> PageShift;

    Span* span = new_span(page_id, ask);
    
    insert_to_free(span);

    return true;
}


}
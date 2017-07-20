//
// central_freelist.cpp
//

#include <cassert>

#include "central_freelist.hpp"
#include "statics.hpp"

namespace core::memory {
void CentralFreeList::Init(int c_idx)
{
    size_class_ = c_idx;
    num_spans_ = 0;
    counter_ = 0;

    empty_.prev = empty_.next = &empty_;
    non_empty_.prev = non_empty_.next = &non_empty_;

    max_cache_size_ = max_num_tc_entries;
    cache_size_ = 16;
    used_slots_ = 0;

    if (c_idx > 0) {
        int size = Static::size_map().ClassToSize(c_idx);
        int num_to_move = Static::size_map().NumObjectsToMove(c_idx);

        max_cache_size_ = (std::min)(max_cache_size_,
            (std::max)(1, 1024 * 1024 / (size * num_to_move)));
        cache_size_ = (std::min)(cache_size_, max_cache_size_);
    }
}

int CentralFreeList::Remove(int n, void** start, void** end)
{
    // let's assume n is NumObjectsToMove
    if (n <= 0) return 0; // invalid case

    //lock_.lock();
    if (used_slots_ > 0) {
        int slot = --used_slots_;
        *start = tc_slots_[slot].head;
        *end = tc_slots_[slot].tail;
        //lock_.unlock();
        return n;
    }

    int result = 0;
    *start = NULL;
    *end = NULL;

    result = fetch_from_one_spans_safe(n, start, end);
    if (result > 0) {
        while (result < n) {
            int tmp;
            void* head = nullptr;
            void* tail = nullptr;
            tmp = fetch_from_one_spans(n - result, &head, &tail);
            if (tmp <= 0) break;
            result += tmp;
            next_of(tail) = *start;
            *start = head;
        }
    }
    //lock_.unlock();

    return result;
}

void CentralFreeList::Insert(int n, void* start, void* end)
{
    lock_.lock();
    // TODO
    lock_.unlock();
}

int CentralFreeList::fetch_from_one_spans(int n, void** start, void** end)
{
    // require lock_
    if (non_empty_.next == &non_empty_) {
        return 0;
    }
    Span* span = non_empty_.next;

    assert(span->objects != nullptr);

    int ret = 0;
    void* prev;
    void* curr;
    curr = span->objects;
    do {
        prev = curr;
        curr = next_of(curr);
    } while (++ret < n && curr != nullptr);

    // when span became empty
    if (curr == nullptr) {
        span->prev->next = span->next;
        span->next->prev = span->prev;
        
        span->next = empty_.next;
        span->prev = &empty_;

        empty_.next->prev = span;
        empty_.next = span;
    }

    *start = span->objects;
    *end = prev;
    span->objects = curr;
    next_of(*end) = nullptr;

    span->ref_count += ret;
    counter_ -= ret;
    return ret;
}

int CentralFreeList::fetch_from_one_spans_safe(int n, void** start, void** end)
{
    // require lock_
    int ret = fetch_from_one_spans(n, start, end);
    if (ret <= 0) {
        populate();
        ret = fetch_from_one_spans(n, start, end);
    }
    return ret;
}

void CentralFreeList::populate()
{
    // require lock_
    //lock_.unlock();
    const int num_pages = Static::size_map().ClassToPages(size_class_);

    Span* span = nullptr;
    {
        //Static::page_heap_lock().lock();
        span = Static::page_heap().New(num_pages);
        if (span) Static::page_heap().RegisterSizeClass(span, size_class_);
        //Static::page_heap_lock().unlock();
    }

    if (span == nullptr) {
        //lock_.lock();
        return;
    }

    span->ref_count = 0;

    // register page info
    for (int i = 0; i < num_pages; i++) {
        Static::page_heap().SetCachedSizeClass(span->start + i, size_class_);
    }

    // split the span into pieces and make them SLL
    void* tail = &span->objects;
    char* ptr = reinterpret_cast<char*>(span->start << PageShift);
    char* end = ptr + (num_pages << PageShift);
    size_t size = Static::size_map().ClassToSize(size_class_);
    int num = 0;

    while (ptr + size <= end) {
        next_of(tail) = ptr;
        tail = ptr;
        ptr += size;
        num++;
    }
    next_of(tail) = nullptr;

    //lock_.lock();
    
    // append
    span->next = non_empty_.next;
    span->prev = &non_empty_;

    non_empty_.next->prev = span;
    non_empty_.next = span;

    ++num_spans_;
    counter_ += num;
}


}
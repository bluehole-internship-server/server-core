//
// malloc.cpp
//

#include <cassert>

#include "malloc.hpp"
#include "statics.hpp"

namespace core::memory {

void* do_malloc(std::size_t size)
{
    return nullptr;
}

void* Malloc(std::size_t size)
{
    assert(Static::IsInited() == true);
    return do_malloc(size);
}

void Free(void* ptr)
{

}
static Static to_initialize;
}
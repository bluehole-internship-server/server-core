//
// test.cpp
//

#include <iostream>
#include <malloc.hpp>
#include <stdlib.h>

using namespace core::memory;

int main()
{
    void* ptr_small = Malloc(1);
    void* ptr_large = Malloc(512 << 10);

    std::cout << ptr_small << std::endl;
    std::cout << ptr_large << std::endl;

    Free(ptr_small, 1);
    Free(ptr_large, 512 << 10);

    return 0;
}
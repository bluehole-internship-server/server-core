//
// test.cpp
//

#include <iostream>
#include <malloc.hpp>
#include <stdlib.h>

using namespace core::memory;

int main()
{
    std::cout << Malloc(1) << std::endl;
    std::cout << Malloc(512 * 1024) << std::endl;
    return 0;
}
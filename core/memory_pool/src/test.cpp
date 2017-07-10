//
// test.cpp
//
// memory_pool is header-only library
// with core::Pool, core::SingletonPool<>, core::ObjectPool<> 
//

#include <iostream>
#include <cassert>
#include <map>
#include <list>
#include <vector>

#include "memory_pool.hpp"

#include "test_and_bench.h"

int main()
{
    TestStruct T;
    BenchStruct B;

    T.TestObjectPool();
    T.TestFastAllocator();

    std::cout << "May be everything is fine.\n";

    // print results of bench marks

    return 0;
}
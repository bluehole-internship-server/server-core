//
// test.cpp
//
// memory_pool is header-only library
// with core::Pool, core::SingletonPool<>, core::ObjectPool<> 
//

#include <iostream>
#include <iomanip>

#include <cassert>

#include <Windows.h>

#include "memory_pool.hpp"

#include "test_and_bench.h"

#define _64K 65536
#define _32K 32768
#define _16K 16384
#define _8K  8192

constexpr int num_size = 14; // 2^3 ~ 2^16
constexpr int num_routine = 1;
constexpr int num_thread = 8;
constexpr unsigned threshold = 8;

constexpr unsigned sizes[] = { 8, 16, 32, 64, 128, 256, 512, 
                            1024, 2048, 4096, 8192, 16384, 32768, 65536 };
constexpr unsigned num_inner[] = { 1000000, 10000 };


TestStruct T;

template<class callable, class... arguments>
double get_avg(int num_routine, callable&& func, arguments&&... args)
{
    std::function<typename std::result_of<callable(arguments...)>::type()>
        task{ std::bind(std::forward<callable>(func),
            std::forward<arguments>(args)...) };
    
    double ret = 0;
    for (int i = 0; i < num_routine; i++) ret += task();

    return ret / num_routine;
}

void print_result(std::size_t size, double avg_pool, double avg_malloc)
{
    std::cout << "size : " << std::setw(5) << size;
    std::cout << ", " << "pool : " << std::setw(10) << avg_pool << ", "
        << "malloc : " << std::setw(10) << avg_malloc << std::endl;
}

template<int st, int ed>
struct test_functor {
    void test_alloc()
    {
        if (st < ed) {
            double avg_pool = 0.0;
            double avg_malloc = 0.0;

            avg_pool = get_avg(num_routine,
                &BenchStruct::BenchSimpleAlloc,
                &B, num_inner[st / threshold], sizes[st]);
            avg_malloc = get_avg(num_routine,
                &BenchStruct::BenchMallocSimpleAlloc,
                &B, num_inner[st / threshold], sizes[st]);
            
            print_result(sizes[st], avg_pool, avg_malloc);
            test_functor<st + 1, ed>().test_alloc();
        }
    }

    void test_alloc_mt()
    {
        if (st < ed) {
            double avg_pool = 0.0;
            double avg_malloc = 0.0;
            
            avg_pool = get_avg(num_routine,
                &BenchStruct::BenchSimpleAllocMT<sizes[st]>,
                &B, num_inner[st / threshold] / num_thread);
            avg_malloc = get_avg(num_routine,
                &BenchStruct::BenchMallocSimpleAllocMT,
                &B, num_inner[st / threshold] / num_thread,
                sizes[st]);
            
            print_result(sizes[st], avg_pool, avg_malloc);
            test_functor<st + 1, ed>().test_alloc_mt();
        }
    }

    void test_alloc_dealloc()
    {
        if (st < ed) {
            double avg_pool = 0.0;
            double avg_malloc = 0.0;

            avg_pool = get_avg(num_routine,
                &BenchStruct::BenchSimpleAllocDealloc,
                &B, num_inner[st / threshold],
                sizes[st]);
            avg_malloc = get_avg(num_routine,
                &BenchStruct::BenchMallocSimpleAllocDealloc,
                &B, num_inner[st / threshold],
                sizes[st]);

            print_result(sizes[st], avg_pool, avg_malloc);
            test_functor<st + 1, ed>().test_alloc_dealloc();
        }
    }

    void test_alloc_dealloc_mt()
    {
        if (st < ed) {
            double avg_pool = 0.0;
            double avg_malloc = 0.0;

            avg_pool = get_avg(num_routine,
                &BenchStruct::BenchSimpleAllocDeallocMT<sizes[st]>,
                &B, num_inner[st / threshold] / num_thread);
            avg_malloc = get_avg(num_routine,
                &BenchStruct::BenchMallocSimpleAllocDeallocMT,
                &B, num_inner[st / threshold] / num_thread,
                sizes[st]);

            print_result(sizes[st], avg_pool, avg_malloc);
            test_functor<st + 1, ed>().test_alloc_dealloc_mt();
        }
    }

    static BenchStruct B;
};

template<int st, int ed>
BenchStruct test_functor<st, ed>::B(num_thread);

template <int ed>
struct test_functor<ed, ed> {
    void test_alloc() { std::cout << std::endl; }
    void test_alloc_mt() { std::cout << std::endl; }

    void test_alloc_dealloc() { std::cout << std::endl; }
    void test_alloc_dealloc_mt() { std::cout << std::endl; }
};

int main()
{
    T.TestObjectPool();
    //T.TestFastAllocator();

    std::cout << "May be everything is fine.\n\n";
   
    std::cout << "test_alloc()\n\n";
    test_functor<0, num_size>().test_alloc();

    std::cout << "test_alloc_mt()\n\n";
    test_functor<0, num_size>().test_alloc_mt();

    std::cout << "test_alloc_dealloc()\n\n";
    test_functor<0, num_size>().test_alloc_dealloc();

    std::cout << "test_alloc_dealloc_mt()\n\n";
    test_functor<0, num_size>().test_alloc_dealloc_mt();

    std::getchar();

    return 0;
}
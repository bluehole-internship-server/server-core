//
// test_and_bench.h
//

#pragma once

#include <functional>
#include <unordered_set>
#include <chrono>

struct TestStruct {
public:
    void TestObjectPool()
    {
        object_pool_.PurgeMemory();
        ptr_set_.clear();

        const int num_routine = 1000;
        int count = 0;

        for (int i = 0; i < num_routine; i++) {
            ptr_set_.insert(object_pool_.Construct(TestClass(count)));
        }

        assert(ptr_set_.size() == num_routine);
        assert(count == num_routine * 2);
    }

    void TestFastAllocator()
    {
        std::list<TestClass, core::FastPoolAllocator<TestClass>> lst;

        const int num_routine = 1000;
        int count = 0;

        for (int i = 0; i < num_routine; i++) {
            lst.push_back(TestClass(count));
        }

        assert(count == num_routine * 2);
        assert(lst.size() == num_routine);
    }

private:
    class TestClass {
    public:
        TestClass(int &count) : count_(count)
        {
            count_++;
        }

        TestClass(TestClass &a) : count_(a.count_)
        {
        }

        TestClass(TestClass &&a) : count_(a.count_)
        {
            count_++;
        }

        ~TestClass() { }

        int &count_;
    };

    core::ObjectPool<TestClass> object_pool_;
    std::unordered_set<void*> ptr_set_;
};

struct BenchStruct {
public:
    double BenchSimpleAlloc(int num_routine, std::size_t size)
    {
        core::Pool pool(size);
        return check_time(std::bind(&BenchStruct::simple_allocate,
            this, num_routine, pool));
    }

    double BenchSimpleAllocDealloc(int num_routine, std::size_t size)
    {
        core::Pool pool(size);
        return check_time(std::bind(&BenchStruct::simple_allocate_deallocate,
            this, num_routine, pool));
    }

    double BenchMallocSimpleAlloc(int num_routine, std::size_t size)
    {
        double ret;
        void** ptr = new void*[num_routine];
        ret = check_time(std::bind(&BenchStruct::malloc_simple_allocate,
            this, num_routine, size, ptr));

        for (int i = 0; i < num_routine; i++) {
            // need to deallocate
            free(ptr[i]);
        }
        delete[] ptr;

        return ret;
    }

    double BenchMallocSimpleAllocDealloc(int num_routine, std::size_t size)
    {
        return check_time(std::bind(
            &BenchStruct::malloc_simple_allocate_deallocate,
            this, num_routine, size));
    }

private:
    void simple_allocate(int num_routine, core::Pool &pool);
    void simple_allocate_deallocate(int num_routine, core::Pool &pool);

    void malloc_simple_allocate(int num_routine, std::size_t size, void** ptr);
    void malloc_simple_allocate_deallocate(int num_routine, std::size_t size);

    double check_time(std::function<void()> func);

    class Dummy64 { long long dummy[8]; };
    class Dummy32 { long long dummy[4]; };
    class Dummy16 { long long dummy[2]; };
    class Dummy8 { long long dummy; };
};


void BenchStruct::simple_allocate(int num_routine, core::Pool &pool)
{
    for (int i = 0; i < num_routine; i++) {
        pool.Malloc();
    }
}

void BenchStruct::simple_allocate_deallocate(int num_routine, core::Pool &pool)
{
    for (int i = 0; i < num_routine; i++) {
        void* ptr = pool.Malloc();
        pool.Free(ptr);
    }
}

void BenchStruct::malloc_simple_allocate(int num_routine,
    std::size_t size, void** ptr)
{
    for (int i = 0; i < num_routine; i++) {
        ptr[i] = malloc(size);
    }
}

void BenchStruct::malloc_simple_allocate_deallocate(int num_routine,
    std::size_t size)
{
    for (int i = 0; i < num_routine; i++) {
        void* ptr = malloc(size);
        free(ptr);
    }
}

double BenchStruct::check_time(std::function<void()> func)
{
    std::chrono::high_resolution_clock::time_point start;
    std::chrono::high_resolution_clock::time_point end;

    start = std::chrono::high_resolution_clock::now();
    func();
    end = std::chrono::high_resolution_clock::now();

    return std::chrono::duration_cast<std::chrono::duration<double,std::milli>>
        (end-start).count();
}
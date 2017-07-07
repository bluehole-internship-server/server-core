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

#include "memory_pool.hpp"

struct ObjectPoolTest {
public:
    void TestConstruct()
    {
        object_pool_.PurgeMemory();
        
        const int num_routine = 1000;
        int count = 0;

        for (int i = 0; i < num_routine; i++) {
            object_pool_.Construct(TestClass(count));
        }
        assert(count == 2000);
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
};

int main()
{
    ObjectPoolTest T;

    T.TestConstruct();
    std::cout << "May be everything is fine.\n";
    return 0;
}
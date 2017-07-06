//
// test.cpp
//
// memory_pool is header-only library
// with core::Pool, core::SingletonPool<>, core::ObjectPool<> 
//

#include <iostream>
#include <cassert>

#include "memory_pool.hpp"

struct ObjectPoolTest {
public:
    void TestConstruct()
    {
        object_pool_.PurgeMemory();
        
        const int num_routine = 1000;
        int count = 0;

        for (int i = 0; i < num_routine; i++) {
            object_pool_.Construct(A(count));
        }
        assert(count == 1000);
    }

private:
    class A {
    public:
        A(int &count) : count_(count) 
        { 
            count_++;
        }

        A(A &a) : count_(a.count_)
        {
        }
        
        A(A &&a) : count_(a.count_)
        {
        }
        
        ~A() { }
        int &count_;
    };
    core::ObjectPool<A> object_pool_;
};


int main()
{
    ObjectPoolTest T;
    T.TestConstruct();
    std::cout << "May be everything is fine.\n";
    return 0;
}
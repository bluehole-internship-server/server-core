//
// object_pool.hpp
//

#pragma once

#include "pool.hpp"
#include "null_mutex.hpp"

namespace core {

template<typename T,
    unsigned RequestedSize = sizeof(T),
    typename Mutex = null_mutex>
class ObjectPool : protected Pool {
public:
    explicit ObjectPool() : Pool(RequestedSize) { }
    ~ObjectPool() {}
    
    T* Malloc()
    {
        std::lock_guard<Mutex> g(mutex_);
        return static_cast<T*>(pool().Malloc());
    }

    void Free(void* const chunk)
    {
        std::lock_guard<Mutex> g(mutex_);
        pool().Free(chunk);
    }

    
    T* Construct(T&& t)
    {   // T needs to implement constructor with r value ref
        // how about using template function?
        std::lock_guard<Mutex> g(mutex_);
        T* ret = static_cast<T*>(pool().Malloc());
        
        try {
            new (ret) T(std::move(t));
        } catch (...) {
            pool().Free(ret);
            ret = nullptr;
        }
        return ret;
    }

    void Destroy(T* const chunk)
    {
        chunk->~T();
        pool().Free(chunk);
    }

    bool PurgeMemory()
    {
        std::lock_guard<Mutex> g(mutex_);
        return pool().PurgeMemory();
    }

private:
    Pool& pool()
    {
        return *this;
    }

    Mutex mutex_;
};
}
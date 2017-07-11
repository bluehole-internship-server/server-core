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
class ObjectPool : public Pool {
public:
    explicit ObjectPool() : Pool(RequestedSize) { }
    ~ObjectPool() {}
    
    virtual void* Malloc() override
    {
        std::lock_guard<Mutex> g(mutex_);
        return static_cast<T*>(Pool::Malloc());
    }

    virtual void Free(void* const chunk) override
    {
        std::lock_guard<Mutex> g(mutex_);
        Pool::Free(chunk);
    }

    
    T* Construct(T&& t)
    {   // T needs to implement constructor with r value ref
        // how about using template function?
        std::lock_guard<Mutex> g(mutex_);
        T* ret = static_cast<T*>(Pool::Malloc());
        
        try {
            new (ret) T(std::move(t));
        } catch (...) {
            Pool::Free(ret);
            ret = nullptr;
        }
        return ret;
    }

    void Destroy(T* const chunk)
    {
        chunk->~T();
        Pool::Free(chunk);
    }

    bool PurgeMemory()
    {
        std::lock_guard<Mutex> g(mutex_);
        return Pool::PurgeMemory();
    }

private:
    /* deprecated
    Pool& pool()
    {
        return *this;
    }
    */

    Mutex mutex_;
};
}
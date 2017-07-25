//
// object_pool.hpp
//

#pragma once

#include "pool.hpp"
#include "null_mutex.hpp"

namespace core {

template<typename T,
    unsigned RequestedSize = sizeof(T)>
class ObjectPool : public Pool {
public:
    explicit ObjectPool() : Pool(RequestedSize) { }
    ~ObjectPool() { }

    inline virtual void* Malloc() override
    {
        return object_pool()->malloc();
    }

    inline virtual void Free(void* const chunk) override
    {
        return object_pool()->free(chunk);
    }

    T* Construct(T&& t)
    {
        T* ret =
            static_cast<T*>(reinterpret_cast<Pool*>(object_pool())->Malloc());

        try {
            new (ret) T(std::move(t));
        }
        catch (...) {
            Pool::Free(ret);
            ret = nullptr;
        }

        return ret;
    }

    inline void Destroy(T* const chunk)
    {
        destroy(chunk);
    }

    inline bool PurgeMemory()
    {
        return object_pool()->purge_memory();
    }

private:
    inline ObjectPool<T, RequestedSize>* object_pool()
    {
        if (object_pool_ == nullptr) {
            object_pool_ = reinterpret_cast<ObjectPool<T, RequestedSize>*>
                (_aligned_malloc(sizeof(ObjectPool<T, RequestedSize>), 64));
            new (object_pool_)ObjectPool<T, RequestedSize>();
        }

        return object_pool_;
    }

    static __declspec(thread) __declspec(align(64))
         ObjectPool<T, RequestedSize>* object_pool_;

    inline void* malloc()
    {
        return Pool::Malloc();
    }

    inline void free(void* const chunk)
    {
        Pool::Free(chunk);
    }

    void destroy(T* const chunk)
    {
        chunk->~T();
        Pool::Free(chunk);
    }

    inline bool purge_memory()
    {
        return Pool::PurgeMemory();
    }
};

template<typename T, unsigned RequestedSize>
typename ObjectPool<T, RequestedSize>*
        ObjectPool<T, RequestedSize>::object_pool_;
}
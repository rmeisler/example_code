#pragma once

//#define USE_STL

#ifdef USE_STL

#include <concurrent_queue.h>

// MSVC STL concurrent queue
template <typename T>
class ConcurrentQueue : public Concurrency::concurrent_queue<T*> {};

#else

#include <queue>
#include "SpinLock.hpp"

// Simple, thread-safe queue
template <typename T>
class ConcurrentQueue
{
public:

    ConcurrentQueue() : mMutex(0) {}

    void push(T* item)
    {
        SpinLock _(mMutex);
        mQueue.push(item);
    }

    bool try_pop(T*& item)
    {
        item = NULL;

        SpinLock _(mMutex);
        if( !mQueue.empty() )
        {
            item = mQueue.front();
            mQueue.pop();

            return true;
        }

        return false;
    }

private:

    std::queue<T*> mQueue;
    LightMutex mMutex;

};

#endif

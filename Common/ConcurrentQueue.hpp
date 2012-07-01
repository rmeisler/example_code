#pragma once

//#define USE_STL

#ifdef USE_STL

#include <concurrent_queue.h>

// MSVC STL concurrent queue
template <typename T>
class ConcurrentQueue : public Concurrency::concurrent_queue<T*> {};

#else

#include "ScopedLock.hpp"

#include <queue>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

// Simple, thread-safe queue
template <typename T>
class ConcurrentQueue
{
public:

    ConcurrentQueue()
    {
        InitializeCriticalSection(&mMutex);
    }

    ~ConcurrentQueue()
    {
        DeleteCriticalSection(&mMutex);
    }

    void push(T* item)
    {
        ScopedLock _(mMutex);
        mQueue.push(item);
    }

    bool try_pop(T*& item)
    {
        item = NULL;

        ScopedLock _(mMutex);
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
    CRITICAL_SECTION mMutex;

};

#endif

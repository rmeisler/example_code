#include "SpinLock.hpp"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

SpinLock::SpinLock(LightMutex& mutex) : mutex(mutex)
{
    // Spin until the mutex is no longer in use
    // (This is not keeping track of recursive grabs, so don't try to grab this lock twice before releasing it)
    while( InterlockedCompareExchange(&mutex, 1, 0) != 1 )
    {
        // If you are doing a spinloop, always insert one of these!
        _mm_pause();
    }
}

SpinLock::~SpinLock()
{
    InterlockedExchange(&mutex, 0);
}

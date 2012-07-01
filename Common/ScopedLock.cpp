#include "ScopedLock.hpp"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

ScopedLock::ScopedLock(_RTL_CRITICAL_SECTION& mutex) : mutex(mutex)
{
    EnterCriticalSection(&mutex);
}

ScopedLock::~ScopedLock()
{
    LeaveCriticalSection(&mutex);
}

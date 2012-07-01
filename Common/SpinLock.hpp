#pragma once

typedef volatile long LightMutex;

// Simple scoped SpinLock for actor factory
// (Short rant on SpinLock...)
// You should 99% of the time be using spinlocks!
// Forget lockless data structures, they are too complex and will almost always have too much overhead!
// Forget CRITICAL_SECTION, it is nearly 100 bytes vs 4 bytes for a LightMutex. CRITICAL_SECTION is a
// heavy weight lock that tries to "be everything for everyone" by both spinning and sleeping after awhile.
// CRITICAL_SECTION is probably best used for something like a file lock. The timing on a file lock can
// change drastically based on whether a file is cached in memory or not. For pretty much any other situation
// SpinLock is a much better choice.
class SpinLock
{
public:

    SpinLock(LightMutex& mutex);
    ~SpinLock();

private:

    LightMutex& mutex;

};

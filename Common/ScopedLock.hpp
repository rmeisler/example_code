#pragma once

// This is just a CRITICAL_SECTION, the underlying type is _RTL_XXX
// I'm just using this so I can forward declare and avoid windows include
// in a header
struct _RTL_CRITICAL_SECTION;

class ScopedLock
{
public:

    ScopedLock(_RTL_CRITICAL_SECTION& mutex);
    ~ScopedLock();

private:

    _RTL_CRITICAL_SECTION& mutex;

};

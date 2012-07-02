#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

__declspec(thread) double freq;
__declspec(thread) __int64 start;

void InitTimer()
{
    LARGE_INTEGER temp;
    QueryPerformanceFrequency(&temp);
    freq = double(temp.QuadPart) / 1000.0;

    QueryPerformanceCounter(&temp);
    start = temp.QuadPart;
}

void StartTime()
{
    LARGE_INTEGER temp;
    QueryPerformanceCounter(&temp);
    start = temp.QuadPart;
}

double GetTime()
{
    LARGE_INTEGER temp;
    QueryPerformanceCounter(&temp);
    return double(temp.QuadPart - start) / freq;
}

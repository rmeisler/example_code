#include "Task.hpp"
#include "AlignedAlloc.hpp"
#include "TaskList.hpp"
#include "Scheduler.hpp"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

void TaskListBase::Dec()
{
    // Decrement shared task count (InterlockedDec, don't want to miss an update to this value!)
    InterlockedDecrement(&mTaskCount);
}

void Task::Done()
{
    mParentList->Dec();
}

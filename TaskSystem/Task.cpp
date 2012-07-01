#include "Task.hpp"
#include "AlignedAlloc.hpp"
#include "TaskList.hpp"
#include "Scheduler.hpp"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

void TaskListBase::Dec(bool waiting)
{
    if( InterlockedDecrement(&mTaskCount) == 0 && !waiting )
    {
        gScheduler->AddCompletedTaskList(this);
    }
}

void Task::Done()
{
    mParentList->Dec(mWaiting);
}

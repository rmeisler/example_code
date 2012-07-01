#include "Scheduler.hpp"
#include "TaskList.hpp"
#include "AlignedAlloc.hpp"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

Scheduler* gScheduler = new Scheduler();

void InitTimer();

Thread::Thread(Scheduler* scheduler) : mRunning(true), mScheduler(scheduler)
{
    mThreadHnd = CreateThread(0, 0, ThreadFunction, this, 0, 0);
}

Thread::~Thread()
{
    mRunning = false;

    WaitForSingleObject(mThreadHnd, 1000);
    CloseHandle(mThreadHnd);
}

DWORD WINAPI Thread::ThreadFunction(LPVOID data)
{
    auto thread = (Thread*)data;

    // We should initialize each timer per thread, in case our frequency differs per core
    InitTimer();

    while( thread->mRunning )
    {
        thread->mScheduler->DoWork();
    }

    return 0;
}

Scheduler::Scheduler()
{
    // Get number of processors
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    unsigned int numberOfThreads = sysinfo.dwNumberOfProcessors - 1; // Main thread is exemplified

    // Create semaphore (Make sure it's got plenty of counts, but not too many, this is a signed number)
    mSemaphore = CreateSemaphore(0, 0, 0xFFFF, 0);

    // Spawn thread pool
    for( unsigned int i = 0; i < numberOfThreads; i++ )
    {
        mThreadPool.push_back(new Thread(this));
    }
}

Scheduler::~Scheduler()
{
    CloseHandle(mSemaphore);

    for( auto itr = mThreadPool.begin(); itr != mThreadPool.end(); itr++ )
    {
        delete *itr;
    }
}

void Scheduler::Update()
{
    TaskListBase* taskList = NULL;

    while( mCompletedTaskLists.try_pop(taskList) )
    {
        taskList->OnComplete();
    }
}

void Scheduler::AddCompletedTaskList(TaskListBase* taskList)
{
    mCompletedTaskLists.push(taskList);
}

void Scheduler::DoWork(unsigned int timeout)
{
    if( WaitForSingleObject(mSemaphore, timeout) != WAIT_TIMEOUT )
    {
        Task* task = NULL;

        if( mQueuedTasks.try_pop(task) )
        {
            task->Run();
            task->Done();
        }
    }
}

void Scheduler::Submit(Task* task)
{
    mQueuedTasks.push(task);
    ReleaseSemaphore(mSemaphore, 1, 0);
}

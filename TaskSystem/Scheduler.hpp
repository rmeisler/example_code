#pragma once

#include <vector>
#include "ConcurrentQueue.hpp"

class Task;
class TaskListBase;
class Scheduler;

class Thread
{
public:

    // Need to pass Scheduler in and use local reference since
    // global reference won't have been set until Scheduler ctor
    // returns (And we are creating thread pool in Scheduler ctor)
    Thread(Scheduler* scheduler);
    ~Thread();

private:

    static unsigned long __stdcall ThreadFunction(void* data);
    
    void* mThreadHnd;
    volatile bool mRunning;
    Scheduler* mScheduler;

};

class Scheduler
{
public:

    Scheduler();
    ~Scheduler();

    void Update();
    void DoWork(unsigned int timeout = -1);
    void Submit(Task* task);

private:

    ConcurrentQueue<Task> mQueuedTasks;

    std::vector<Thread*> mThreadPool;
    void* mSemaphore;

};

extern Scheduler* gScheduler;
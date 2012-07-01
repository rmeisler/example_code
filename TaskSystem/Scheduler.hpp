#pragma once

#include <vector>
#include "ConcurrentQueue.hpp"

class Task;
class TaskListBase;
class Scheduler;

class Thread
{
public:

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
    void AddCompletedTaskList(TaskListBase* taskList);
    void DoWork(unsigned int timeout = -1);
    void Submit(Task* task);

private:

    ConcurrentQueue<TaskListBase> mCompletedTaskLists;
    ConcurrentQueue<Task> mQueuedTasks;

    std::vector<Thread*> mThreadPool;
    void* mSemaphore;

};

extern Scheduler* gScheduler;
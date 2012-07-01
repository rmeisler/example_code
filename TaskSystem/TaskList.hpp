#pragma once

#include "Task.hpp"

#include <vector>

class Scheduler;
extern Scheduler* gScheduler;

class TaskListBase
{
public:

    TaskListBase() : mTaskCount(0) {}
    virtual ~TaskListBase() {}

    virtual void OnComplete() = 0;

    void Dec(bool waiting);
    
protected:

    volatile unsigned long mTaskCount;

};

// Template is simply to ensure that every task added
// to this list is the same type. Having a task list
// run multiple different types of tasks doesn't make
// any sense, it's purpose is to group similar tasks!
// Just use a different task list!
template <typename TaskType>
class TaskList : public TaskListBase
{
public:

    virtual void OnComplete(const std::vector<TaskType*>& tasks) = 0;

    void Add(TaskType* task)
    {
        // TODO: Add debug assert here preventing user from adding
        // tasks to task list after it's been submitted
        mTasks.push_back(task);
        mTaskCount++;

        task->mParentList = this;
    }

    void Submit()
    {
        // TODO: Add debug assert here preventing double submit
        for( auto itr = mTasks.begin(); itr != mTasks.end(); itr++ )
        {
            gScheduler->Submit(*itr);
        }
    }

    void SubmitAndWait()
    {
        for( auto itr = mTasks.begin(); itr != mTasks.end(); itr++ )
        {
            (*itr)->mWaiting = true;
        }

        Submit();
        
        while( mTaskCount > 0 )
        {
            gScheduler->DoWork(0);

            // If you are doing a spinloop, always insert one of these!
            _mm_pause();
        }

        OnComplete(mTasks);
        Cleanup();
    }

    std::vector<TaskType*>& GetResults()
    {
        return mTasks;
    }

private:

    void Cleanup()
    {
        // This is the place where we can cleanup all the memory
        for( auto itr = mTasks.begin(); itr != mTasks.end(); itr++ )
        {
            AlignedFree(*itr);
        }

        // This ensures that tasklists are one-shot objects
        delete this;
    }

    virtual void OnComplete()
    {
        OnComplete(mTasks);
        Cleanup();
    }

    std::vector<TaskType*> mTasks;

};

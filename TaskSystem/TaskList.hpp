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

    void Dec();
    
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

    ~TaskList()
    {
        // Cleanup tasks
        for( auto itr = mTasks.begin(); itr != mTasks.end(); itr++ )
        {
            delete *itr;
        }
    }
    
    void Add(TaskType* task)
    {
        // TODO: Add debug assert here preventing user from adding
        // tasks to task list after it's been submitted
        mTasks.push_back(task);
        mTaskCount++;

        task->mParentList = this;
    }

    void SubmitAndWait()
    {
        for( auto itr = mTasks.begin(); itr != mTasks.end(); itr++ )
        {
            gScheduler->Submit(*itr);
        }
        
        while( mTaskCount > 0 )
        {
            gScheduler->DoWork(true);

            // If you are doing a spinloop, always insert one of these so the OS knows it!
            YieldProcessor();
        }
    }

    std::vector<TaskType*>& GetResults()
    {
        return mTasks;
    }

private:
    
    std::vector<TaskType*> mTasks;

};

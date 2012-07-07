#pragma once

class TaskListBase;

// Try removing this and running in release mode, you may see some terrible performance.
// This is the result of cache invalidation from false sharing.
__declspec(align(64))
class Task
{
public:

    virtual ~Task() {}

    virtual void Run() = 0;
    void Done();

private:

    template <typename TaskType>
    friend class TaskList;

    TaskListBase* mParentList;

};

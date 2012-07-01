#pragma once

class TaskListBase;

__declspec(align(64))
class Task
{
public:

    Task() : mWaiting(false) {}
    virtual ~Task() {}

    virtual void Run() = 0;
    void Done();

private:

    template <typename TaskType>
    friend class TaskList;

    TaskListBase* mParentList;

protected:

    bool mWaiting;

};

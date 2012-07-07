#pragma once

class TaskListBase;

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

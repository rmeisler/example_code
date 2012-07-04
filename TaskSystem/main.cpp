/////////////////////////////////////////////////////////////////////
// The following code approximates PI using both serial and parallel
// implementations. PI approximation is an "embarrasingly parallel"
// algorithm, why is why I'm using it to demonstrate the use of a
// Task Management system. Though this is fairly contrived, it's not
// unlike how you might use tasks for physics broadphase/narrowphase.
/////////////////////////////////////////////////////////////////////
#include "Scheduler.hpp"
#include "TaskList.hpp"
#include "AlignedAlloc.hpp"

#include <stdio.h>

void InitTimer();
double GetTime();

class PiApproxTask : public Task
{
public:

    PiApproxTask(int start, int end) : acc(0.0), start(start), end(end) {}

    virtual void Run()
    {
        for( int i = start; i < end; i++ )
        {
            acc += 4.0 * (1.0 - (i % 2) * 2) / (2 * i + 1);
        }
    }

    double acc;
    int start;
    int end;

};

class PiApproxTaskList : public TaskList<PiApproxTask>
{
public:

    virtual void OnComplete(const std::vector<PiApproxTask*>& tasks)
    {
        double piCalc = 0.0;
        for( auto itr = tasks.begin(); itr != tasks.end(); itr++ )
        {
            piCalc += (*itr)->acc;
        }
        
        printf("Pi = %f\nThis took %f milliseconds!\n"
               "Try running this multiple times to get an average time", piCalc, GetTime());
    }

};

void Parallel()
{
    auto taskList = new PiApproxTaskList();
    
    for( int i = 0; i < 4; i++ )
    {
        taskList->Add(AlignedAlloc<PiApproxTask>(i * 10000000, (i + 1) * 10000000 - 1));
    }
    
    taskList->SubmitAndWait();

    // Warning! In case you missed it in TaskList.hpp, I've designed TaskLists to be one-shot objects
    // that cleanup all their tasks memory and their own after completing. This is a very efficient implementation,
    // but also error prone. You can avoid this implementation if you use reference counting on Tasks/TaskLists
}

void Serial()
{
    double acc = 0.0;
    int start = 0, end = 40000000;
    for( int i = start; i < end; i++ )
    {
        acc += 4.0 * (1.0 - (i % 2) * 2) / (2 * i + 1);
    }

    printf("Pi = %f\nThis took %f milliseconds!\n"
           "Try running this multiple times to get an average time", acc, GetTime());
}

void main()
{
    InitTimer();

    Parallel();

    system("pause");
}

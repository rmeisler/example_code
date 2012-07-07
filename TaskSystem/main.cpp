/////////////////////////////////////////////////////////////////////
// The following code approximates PI using both serial and parallel
// implementations. PI approximation is an "embarrasingly parallel"
// algorithm, why is why I'm using it to demonstrate the use of a
// Task Management system. Though this is fairly contrived, it's not
// unlike how you might use tasks for physics broadphase/narrowphase.
/////////////////////////////////////////////////////////////////////
#include "Scheduler.hpp"
#include "TaskList.hpp"

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

void Parallel()
{
    auto taskList = new TaskList<PiApproxTask>();
    PiApproxTask* tasks[4];
    
    for( int i = 0; i < 4; i++ )
    {
        tasks[i] = new PiApproxTask(i * 10000000, (i + 1) * 10000000 - 1);
    }
    
    // I do this to illustrate the overhead of actually dynamically allocating tasks,
    // you should actually pool tasks, as it will give you a good performance boost!
    // This is where declspec align becomes super important, if tasks are explicitly
    // next to each other in memory and used on separate cores, we MUST use alignment
    // to avoid false sharing.
    InitTimer();

    for( int i = 0; i < 4; i++ )
    {
      taskList->Add(tasks[i]);
    }

    taskList->SubmitAndWait();
    
    double piCalc = 0.0;
    auto results = taskList->GetResults();
    for( auto itr = results.begin(); itr != results.end(); itr++ )
    {
        piCalc += (*itr)->acc;
    }

    printf("Pi = %f\nThis took %f milliseconds!\n"
            "Try running this multiple times", piCalc, GetTime());

    delete taskList;
}

void Serial()
{
    InitTimer();
    double acc = 0.0;
    int start = 0, end = 40000000;
    for( int i = start; i < end; i++ )
    {
        acc += 4.0 * (1.0 - (i % 2) * 2) / (2 * i + 1);
    }

    printf("Pi = %f\nThis took %f milliseconds!\n"
           "Try running this multiple times", acc, GetTime());
}

void main()
{
    Parallel();

    system("pause");
}

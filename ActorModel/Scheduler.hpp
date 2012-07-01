#pragma once

#include <vector>
#include "ConcurrentQueue.hpp"

class Message;
class Actor;

class Thread
{
public:

    Thread();
    ~Thread();

    void Join(unsigned int timeout = -1);
    void Enqueue(Message* msg);

private:

    static unsigned long __stdcall ThreadFunction(void* data);
    
    void* mThreadHnd;
    ConcurrentQueue<Message> mQueue;
    void* mSemaphore;
    volatile bool mRunning;

};

class Scheduler
{
public:

    Scheduler();
    ~Scheduler();

    void AssignToThread(Actor* actor);
    void Send(Message* msg);
    void DedicateMainThread();
    
private:

    std::vector<Thread*> mThreadPool;
    unsigned int mCurrentThreadIndex;

};

extern Scheduler* gScheduler;

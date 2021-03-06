#include "Scheduler.hpp"
#include "CreateActorMsg.hpp"
#include "ActorFactory.hpp"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

Scheduler* gScheduler = new Scheduler();

void InitTimer();

Thread::Thread() : mRunning(true)
{
    mThreadHnd = CreateThread(0, 0, ThreadFunction, this, 0, 0);
    mSemaphore = CreateSemaphore(0, 0, 0xFFFF, 0);
}

Thread::~Thread()
{
    mRunning = false;

    WaitForSingleObject(mThreadHnd, 1000);
    CloseHandle(mThreadHnd);
    CloseHandle(mSemaphore);
}

void Thread::Join(unsigned int timeout)
{
    WaitForSingleObject(mThreadHnd, timeout);
}

void Thread::Enqueue(Message* msg)
{
    mQueue.push(msg);
    ReleaseSemaphore(mSemaphore, 1, 0);
}

DWORD WINAPI Thread::ThreadFunction(LPVOID data)
{
    auto thread = (Thread*)data;
    Message* msg = NULL;

    // We should initialize each timer per thread, in case our frequency differs per core
    InitTimer();

    while( thread->mRunning )
    {
        WaitForSingleObject(thread->mSemaphore, INFINITE);

        if( thread->mQueue.try_pop(msg) )
        {
            if( msg->type == CreateActorMsg::id )
            {
                // Construct actor
                gActorFactory->ThreadConstruct(msg->to);
            }
            else
            {
                // ONLY OWNING THREAD SHOULD EVER TOUCH RAW ACTOR DATA
                Actor* ref = gActorFactory->Get(msg->to.id, msg->to.index);
            
                // If this actor ref is alive
                if( ref )
                {
                    ref->HandleMessage(msg);

                    // Free stopped actor
                    if( ref->IsStopped() )
                        gActorFactory->Destroy(msg->to);
                }
            }

            // Free message
            delete msg;
        }
    }

    return 0;
}

Scheduler::Scheduler() : mCurrentThreadIndex(0)
{
    // Get number of processors
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    unsigned int numberOfThreads = sysinfo.dwNumberOfProcessors - 1; // Main thread is exemplified

    // Spawn thread pool
    for( unsigned int i = 0; i < numberOfThreads; i++ )
    {
        mThreadPool.push_back(new Thread());
    }
}

Scheduler::~Scheduler()
{
    // Cleanup threads
    for( auto itr = mThreadPool.begin(); itr != mThreadPool.end(); itr++ )
    {
        delete *itr;
    }
}

void Scheduler::AssignToThread(ActorHandle& actor)
{
    // Round robin scheduling
    actor.threadId = mCurrentThreadIndex;
    mCurrentThreadIndex = (mCurrentThreadIndex + 1) % mThreadPool.size();
}

void Scheduler::Send(Message* msg)
{
    // Figure out which thread to send this to
    unsigned int threadIndex = msg->to.threadId;
    mThreadPool[threadIndex]->Enqueue(msg);
}

void Scheduler::DedicateMainThread()
{
    // TODO: Assert if this is called from anywhere but main thread
    // or if this has already been called
    mThreadPool.push_back(new Thread());

    // Put original version of main thread to sleep,
    // new thread should now take control of the core being used by main thread
    mThreadPool[mThreadPool.size() - 1]->Join();
}

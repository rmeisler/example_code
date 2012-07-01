#include "Actor.hpp"
#include "Scheduler.hpp"
#include "AlignedAlloc.hpp"

Actor::Actor() : mStopped(false)
{
    gScheduler->AssignToThread(this);
}

void Actor::Send(Message* msg)
{
    msg->to = mHandle;

    gScheduler->Send(msg);
}

void Actor::Spawn()
{
    // Send message to kickstart message handler, then stop immediately after handling message
    Send(AlignedAlloc<Message>(0));
    Stop();
}

void Actor::Stop()
{
    mStopped = true;
}

bool Actor::IsStopped() const
{
    return mStopped;
}

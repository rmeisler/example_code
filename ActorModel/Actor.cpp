#include "Actor.hpp"

Actor::Actor() : mStopped(false)
{
    
}

void Actor::Stop()
{
    mStopped = true;
}

bool Actor::IsStopped() const
{
    return mStopped;
}

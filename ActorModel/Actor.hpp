#pragma once

#include "ActorHandle.hpp"

class Message
{
public:

    Message(int type) : type(type) {}

    int type;
    ActorHandle to;

};

// No one should ever call functions on actor accept owning thread
// All access to actor is through ActorHandle, which can only submit messages
class Actor
{
public:
    
    Actor();

    virtual void HandleMessage(Message* msg) = 0;

    void Stop();
    bool IsStopped() const;

private:

    bool mStopped;

};

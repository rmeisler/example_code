#pragma once

#include "ActorHandle.hpp"

__declspec(align(64))
class Message
{
public:

    Message(int type) : type(type) {}

    int type;
    ActorHandle to;

};

__declspec(align(64))
class Actor
{
public:
    
    Actor();

    virtual void HandleMessage(Message* msg) = 0;

    void Send(Message* msg);
    void Spawn();

    void Stop();
    bool IsStopped() const;

private:

    friend class Scheduler;
    friend class ActorFactory;

    ActorHandle mHandle;
    unsigned int mThreadId;
    bool mStopped;

};

#pragma once

class Message;

struct ActorHandle
{
    ActorHandle() : id(0), index(0) {}
    ActorHandle(unsigned int id, unsigned int index);
    
    void Send(Message* msg);

    unsigned int id;
    unsigned int index;
    unsigned int threadId;

};

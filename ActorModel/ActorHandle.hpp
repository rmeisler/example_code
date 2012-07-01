#pragma once

class Actor;
class ActorFactory;

struct ActorHandle
{
    ActorHandle() : factory(0), id(0), index(0) {}
    ActorHandle(ActorFactory* factory, unsigned int id, unsigned int index);
    Actor* Get();

    ActorFactory* factory;
    unsigned int id;
    unsigned int index;
};

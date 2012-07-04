#pragma once

#include <vector>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "ActorHandle.hpp"

class Actor;

// Store data needed to build actor, so we can delay
// creation until its on the parent thread
struct ActorCreator
{
    Actor* (*ctor)(char*);
    char* paramBuffer;
};

// Actor constructor proxies
template <typename ActorType>
Actor* ActorCtor(char* paramBuffer)
{
    return new ActorType();
}

template <typename ActorType, typename Param1Type>
Actor* ActorCtor(char* paramBuffer)
{
    Param1Type& param1 = *(Param1Type*)paramBuffer;
    return new ActorType(param1);
}

template <typename ActorType, typename Param1Type, typename Param2Type>
Actor* ActorCtor(char* paramBuffer)
{
    Param1Type& param1 = *(Param1Type*)paramBuffer;
    Param2Type& param2 = *(Param2Type*)(paramBuffer + sizeof(Param1Type));
    return new ActorType(param1, param2);
}

template <typename ActorType, typename Param1Type, typename Param2Type, typename Param3Type>
Actor* ActorCtor(char* paramBuffer)
{
    Param1Type& param1 = *(Param1Type*)paramBuffer;
    Param2Type& param2 = *(Param2Type*)(paramBuffer + sizeof(Param1Type));
    Param3Type& param3 = *(Param3Type*)(paramBuffer + sizeof(Param1Type) + sizeof(Param2Type));
    return new ActorType(param1, param2, param3);
}

// Simple slot map for managing actor handles
class ActorFactory
{
public:

    ActorFactory();

    template <typename ActorType>
    ActorHandle Create()
    {
        ActorCreator creator;
        creator.ctor = ActorCtor<ActorType>;
        creator.paramBuffer = 0;

        return CreateHandle(creator);
    }

    template <typename ActorType, typename CtorParam1Type>
    ActorHandle Create(const CtorParam1Type& param1)
    {
        ActorCreator creator;
        creator.ctor = ActorCtor<ActorType, CtorParam1Type>;
        creator.paramBuffer = (char*)malloc(sizeof(CtorParam1Type));
        new (creator.paramBuffer) CtorParam1Type(param1);

        return CreateHandle(creator);
    }

    template <typename ActorType, typename CtorParam1Type, typename CtorParam2Type>
    ActorHandle Create(const CtorParam1Type& param1, const CtorParam2Type& param2)
    {
        ActorCreator creator;
        creator.ctor = ActorCtor<ActorType, CtorParam1Type, CtorParam2Type>;
        creator.paramBuffer = (char*)malloc(sizeof(CtorParam1Type) + sizeof(CtorParam2Type));
        new (creator.paramBuffer) CtorParam1Type(param1);
        new (creator.paramBuffer + sizeof(CtorParam1Type)) CtorParam2Type(param2);

        return CreateHandle(creator);
    }

    template <typename ActorType, typename CtorParam1Type, typename CtorParam2Type, typename CtorParam3Type>
    ActorHandle Create(const CtorParam1Type& param1, const CtorParam2Type& param2, const CtorParam3Type& param3)
    {
        ActorCreator creator;
        creator.ctor = ActorCtor<ActorType, CtorParam1Type, CtorParam2Type, CtorParam3Type>;
        creator.paramBuffer = (char*)malloc(sizeof(CtorParam1Type) + sizeof(CtorParam2Type) + sizeof(CtorParam3Type));
        new (creator.paramBuffer) CtorParam1Type(param1);
        new (creator.paramBuffer + sizeof(CtorParam1Type)) CtorParam2Type(param2);
        new (creator.paramBuffer + sizeof(CtorParam1Type) + sizeof(CtorParam2Type)) CtorParam3Type(param3);

        return CreateHandle(creator);
    }

    void Destroy(ActorHandle& handle);

    Actor* Get(unsigned int id, unsigned int index);

    void ThreadConstruct(ActorHandle& handle);

private:

    ActorHandle CreateHandle(ActorCreator& creator);

    struct Index
    {
        // We only need to store one at a time
        union
        {
            ActorCreator creator;
            Actor* actorRef;
        }
        actorRef;

        unsigned int id;
        int nextFreeIndex;
    };

    static const unsigned int FREE_SLOT = 0xF2335107;
    static const unsigned int GROW_SIZE = 32;

    void Grow();

    unsigned int mIdCount;

    // Reader/writer lock (Faster than CS when reading is more common than writing)
    SRWLOCK mMutex;

    int mFreeIndexHead;
    std::vector<Index> mSlots;

};

extern ActorFactory* gActorFactory;

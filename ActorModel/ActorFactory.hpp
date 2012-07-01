#pragma once

#include <vector>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "ActorHandle.hpp"
#include "ScopedLock.hpp"

// Simple slot map for managing actor handles
class ActorFactory
{
public:

    ActorFactory();
    ~ActorFactory();

    template <typename ActorType>
    ActorHandle Create()
    {
        return CreateHandle(AlignedAlloc<ActorType>());
    }

    template <typename ActorType, typename CtorParam1Type>
    ActorHandle Create(const CtorParam1Type& param1)
    {
        return CreateHandle(AlignedAlloc<ActorType>(param1));
    }

    template <typename ActorType, typename CtorParam1Type, typename CtorParam2Type>
    ActorHandle Create(const CtorParam1Type& param1, const CtorParam2Type& param2)
    {
        return CreateHandle(AlignedAlloc<ActorType>(param1, param2));
    }

    template <typename ActorType, typename CtorParam1Type, typename CtorParam2Type, typename CtorParam3Type>
    ActorHandle Create(const CtorParam1Type& param1, const CtorParam2Type& param2, const CtorParam3Type& param3)
    {
        return CreateHandle(AlignedAlloc<ActorType>(param1, param2, param3));
    }

    void Destroy(ActorHandle& handle);

    Actor* Get(unsigned int id, unsigned int index);

private:

    ActorHandle CreateHandle(Actor* actor);

    struct Index
    {
        Actor* actorRef;
        unsigned int id;
        int nextFreeIndex;
    };

    static const unsigned int FREE_SLOT = 0xF2335107;
    static const unsigned int GROW_SIZE = 32;

    void Grow();

    unsigned int mIdCount;

    // TODO: Make a more sophisticated locking scheme for this data structure?
    // e.g. separate into multiple locks for reading/growing and creating/destroying?
    CRITICAL_SECTION mMutex;

    int mFreeIndexHead;
    std::vector<Index> mSlots;

};

extern ActorFactory* gActorFactory;

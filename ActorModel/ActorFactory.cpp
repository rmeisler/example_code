#include "ActorFactory.hpp"
#include "CreateActorMsg.hpp"
#include "Scheduler.hpp"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

ActorFactory* gActorFactory = new ActorFactory();

// WARNING, because of thread assignment, creating an ActorHandle with same id/index
// will NOT result in getting a handle to the same Actor! Use copy ctor!
ActorHandle::ActorHandle(unsigned int id, unsigned int index)
    : id(id), index(index)
{
    gScheduler->AssignToThread(*this);
}

void ActorHandle::Send(Message* msg)
{
    msg->to = *this;
    gScheduler->Send(msg);
}

// Note: Always need to default LightMutex to 0, perhaps a usability downside to making LightMutex a simple typedef
ActorFactory::ActorFactory() : mIdCount(0), mFreeIndexHead(-1)
{
    InitializeSRWLock(&mMutex);

    Grow();
}

ActorHandle ActorFactory::CreateHandle(ActorCreator& creator)
{
    // Lock this whole function
    AcquireSRWLockExclusive(&mMutex);

    // Need to grow pool
    if( mFreeIndexHead == -1 )
        Grow();

    Index* freeIndex = &mSlots[mFreeIndexHead];
    unsigned curIndex = mFreeIndexHead;

    freeIndex->actorRef.creator = creator;
    freeIndex->id = mIdCount++;
    mFreeIndexHead = freeIndex->nextFreeIndex;

    unsigned int newIndex = freeIndex->id;

    ReleaseSRWLockExclusive(&mMutex);

    // Schedule message to construct local copy of actor on thread
    // it is important that we use a local copy as it avoids
    // false sharing!
    Message* msg = new CreateActorMsg();
    msg->to = ActorHandle(newIndex, curIndex);

    gScheduler->Send(msg);

    return msg->to;
}

void ActorFactory::ThreadConstruct(ActorHandle& handle)
{
    // Build actor and overwrite actorRef union
    Index* indexRef = &mSlots[handle.index];
    ActorCreator& creator = indexRef->actorRef.creator;
    
    Actor* newActorRef = creator.ctor(creator.paramBuffer);
    free(creator.paramBuffer);

    indexRef->actorRef.actorRef = newActorRef;
}

void ActorFactory::Destroy(ActorHandle& handle)
{
    // Writer lock
    AcquireSRWLockExclusive(&mMutex);

    Index* index = &mSlots[handle.index];

    delete index->actorRef.actorRef;

    index->actorRef.actorRef = NULL;
    index->id = FREE_SLOT;
    index->nextFreeIndex = mFreeIndexHead;

    mFreeIndexHead = handle.index;

    ReleaseSRWLockExclusive(&mMutex);
}

Actor* ActorFactory::Get(unsigned int id, unsigned int index)
{
    // Reader lock
    AcquireSRWLockShared(&mMutex);

    Index* indexRef = &mSlots[index];

    ReleaseSRWLockShared(&mMutex);

    return indexRef->id == id ? indexRef->actorRef.actorRef : NULL;
}

void ActorFactory::Grow()
{
    unsigned int curIndex = mSlots.size();
    for( unsigned int i = 0; i < GROW_SIZE; i++, curIndex++ )
    {
        Index newIndex;

        newIndex.actorRef.actorRef = NULL;
        newIndex.id = FREE_SLOT;
        newIndex.nextFreeIndex = mFreeIndexHead;
        mFreeIndexHead = curIndex;

        mSlots.push_back(newIndex);
    }
}

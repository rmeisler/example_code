#include "ActorFactory.hpp"
#include "AlignedAlloc.hpp"
#include "Actor.hpp"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

ActorFactory* gActorFactory = new ActorFactory();

ActorHandle::ActorHandle(ActorFactory* factory, unsigned int id, unsigned int index)
    : factory(factory), id(id), index(index)
{

}

Actor* ActorHandle::Get()
{
    // TODO: Debug assert on null factory (i.e. default constructed handle being accessed)
    return factory->Get(id, index);
}

// Note: Always need to default LightMutex to 0, perhaps a usability downside to making LightMutex a simple typedef
ActorFactory::ActorFactory() : mIdCount(0), mFreeIndexHead(-1), mMutex(0)
{
    Grow();
}

ActorHandle ActorFactory::CreateHandle(Actor* actor)
{
    // Lock this whole function
    SpinLock _(mMutex);

    // Need to grow pool
    if( mFreeIndexHead == -1 )
        Grow();

    Index* freeIndex = &mSlots[mFreeIndexHead];
    unsigned curIndex = mFreeIndexHead;

    freeIndex->actorRef = actor;
    freeIndex->id = mIdCount++;
    mFreeIndexHead = freeIndex->nextFreeIndex;

    // Construct new actor handle
    ActorHandle handle(this, freeIndex->id, curIndex);

    // Set internal handle on Actor
    actor->mHandle = handle;

    return handle;
}

void ActorFactory::Destroy(ActorHandle& handle)
{
    // Lock this whole function
    SpinLock _(mMutex);

    Index* index = &mSlots[handle.index];

    AlignedFree(index->actorRef);

    index->actorRef = NULL;
    index->id = FREE_SLOT;
    index->nextFreeIndex = mFreeIndexHead;

    mFreeIndexHead = handle.index;
}

Actor* ActorFactory::Get(unsigned int id, unsigned int index)
{
    // Lock this whole function
    SpinLock _(mMutex);

    Index* indexRef = &mSlots[index];

    return indexRef->id == id ? indexRef->actorRef : NULL;
}

void ActorFactory::Grow()
{
    unsigned int curIndex = mSlots.size();
    for( unsigned int i = 0; i < GROW_SIZE; i++, curIndex++ )
    {
        Index newIndex;

        newIndex.actorRef = NULL;
        newIndex.id = FREE_SLOT;
        newIndex.nextFreeIndex = mFreeIndexHead;
        mFreeIndexHead = curIndex;

        mSlots.push_back(newIndex);
    }
}

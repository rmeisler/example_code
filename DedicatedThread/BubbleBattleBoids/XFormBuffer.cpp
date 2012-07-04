#include "PCH.h"
#include "GameObject.h"

#include <stdlib.h>
#include <utility>

XFormBuffer::XFormBuffer()
{
    mRenderCompleteEvent = CreateEvent(0, 0, 0, 0);
    mBufferSwapEvent = CreateEvent(0, 0, 0, 0);

    mActualBufferSize = 0;
    mDirtyBufferSize = 0;
}

XFormBuffer::~XFormBuffer()
{
    CloseHandle(mRenderCompleteEvent);
    CloseHandle(mBufferSwapEvent);
}

void XFormBuffer::Add(GameObject* object)
{
    ASSERT(mActualBufferSize < BUFFER_CAPACITY);

    // Pass object the index into the buffer it'll need to access its data (xform id)
    object->SetXFormId(mActualBufferSize);

    // Zero out xform object
    memset(mBufferPtrB + mActualBufferSize, 0, sizeof(XFormObject));

    // Set object pointer on xform object
    mBufferPtrB[mActualBufferSize].owner = object;

    // Increase xform buffer size
    mActualBufferSize++;
}

void XFormBuffer::Remove(unsigned int index)
{
    // Swap last item with this item
    std::swap(mBufferPtrB[index], mBufferPtrB[mActualBufferSize - 1]);

    // Shrink buffer size
    mActualBufferSize--;
    
    // Update index of swapped object
    mBufferPtrB[index].owner->SetXFormId(index);
}

XFormObject* XFormBuffer::Get(unsigned int index)
{
    return &mBufferPtrB[index];
}

XFormObject* XFormBuffer::GetRenderBuffer()
{
    return mBufferPtrA;
}

void XFormBuffer::SyncBuffer()
{
    // Signal render event
    SetEvent(mRenderCompleteEvent);
}

void XFormBuffer::CopyBuffer()
{
    WaitForSingleObject(mRenderCompleteEvent, INFINITE);
    
    // Update buffer size
    InterlockedExchange(&mDirtyBufferSize, mActualBufferSize);

    // Swap buffers
    XFormObject* ptrA = mBufferPtrA;
    InterlockedExchangePointer(&mBufferPtrA, mBufferPtrB);
    mBufferPtrB = ptrA;
    
    // We can do memcpy on our own time...

    // Essentially all we're doing is copying the results of the main thread processing over to
    // the render thread's xform buffer
    memcpy(mBufferPtrB, mBufferPtrA, sizeof(XFormObject) * mActualBufferSize);

    // No one should be referencing old dead objects now, cleanup!
    GameObject::CleanUp();
}

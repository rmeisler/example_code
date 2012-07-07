#include "PCH.h"
#include "GameObject.h"

#include <stdlib.h>

XFormBufferManager::XFormBufferManager()
{
    mBufferPtrA = &mBufferA;
    mBufferPtrB = &mBufferB;
    mBufferPtrC = &mBufferC;

    mReadBufferDirty = false;

    mBufferPtrA->size = 0;
    mBufferPtrB->size = 0;
    mBufferPtrC->size = 0;
}

void XFormBufferManager::Add(GameObject* object)
{
    unsigned int bufferSize = mBufferPtrA->size;
    ASSERT(bufferSize < BUFFER_CAPACITY);

    // Pass object the index into the buffer it'll need to access its data (xform id)
    object->SetXFormId(bufferSize);

    // Zero out xform object
    memset(mBufferPtrA->buffer + bufferSize, 0, sizeof(XFormObject));

    // Set object pointer on xform object
    mBufferPtrA->buffer[bufferSize].owner = object;

    // Increase xform buffer size
    mBufferPtrA->size++;
}

void XFormBufferManager::Remove(unsigned int index)
{
    // Swap last item with this item
    std::swap(mBufferPtrA->buffer[index], mBufferPtrA->buffer[mBufferPtrA->size - 1]);

    // Shrink buffer size
    mBufferPtrA->size--;
    
    // Update index of swapped object
    mBufferPtrA->buffer[index].owner->SetXFormId(index);
}

XFormObject* XFormBufferManager::Get(unsigned int index)
{
    return &mBufferPtrA->buffer[index];
}

XFormBuffer* XFormBufferManager::GetReadBuffer() const
{
    return mBufferPtrC;
}

bool XFormBufferManager::ReadBufferDirty() const
{
    return mReadBufferDirty;
}

void XFormBufferManager::SwapReadBuffers()
{
    // Swap buffers
    XFormBuffer* temp = mBufferPtrB;

    // Pivot write needs to be made available on other thread immediately
    InterlockedExchangePointer(&mBufferPtrB, mBufferPtrC);

    // This write only matters to render thread
    mBufferPtrC = temp;
    
    mReadBufferDirty = false;
}

void XFormBufferManager::SwapWriteBuffers()
{
    // Swap buffers
    XFormBuffer* temp = mBufferPtrB;

    // Pivot write needs to be made available on other thread immediately
    InterlockedExchangePointer(&mBufferPtrB, mBufferPtrA);

    // This write only matters to main thread
    mBufferPtrA = temp;

    mReadBufferDirty = true;

    // Essentially all we're doing is copying the results of main thread processing last frame
    // into our new buffer before using it for this new frame
    memcpy(mBufferPtrA, mBufferPtrB, sizeof(XFormObject) * mBufferPtrB->size + sizeof(unsigned int));
}

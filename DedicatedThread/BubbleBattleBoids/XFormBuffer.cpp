#include "PCH.h"
#include "GameObject.h"

#include <stdlib.h>

XFormBufferManager::XFormBufferManager()
{
    mWriteBuffer = &mBufferA;
    mPivotBuffer = &mBufferB;
    mReadBuffer = &mBufferC;

    mReadBufferDirty = false;

    mWriteBuffer->size = 0;
    mPivotBuffer->size = 0;
    mReadBuffer->size = 0;
}

void XFormBufferManager::Add(GameObject* object)
{
    unsigned int bufferSize = mWriteBuffer->size;
    ASSERT(bufferSize < BUFFER_CAPACITY);

    // Pass object the index into the buffer it'll need to access its data (xform id)
    object->SetXFormId(bufferSize);

    // Zero out xform object
    memset(mWriteBuffer->buffer + bufferSize, 0, sizeof(XFormObject));

    // Set object pointer on xform object
    mWriteBuffer->buffer[bufferSize].owner = object;

    // Increase xform buffer size
    mWriteBuffer->size++;
}

void XFormBufferManager::Remove(unsigned int index)
{
    // Swap last item with this item
    std::swap(mWriteBuffer->buffer[index], mWriteBuffer->buffer[mWriteBuffer->size - 1]);

    // Shrink buffer size
    mWriteBuffer->size--;
    
    // Update index of swapped object
    mWriteBuffer->buffer[index].owner->SetXFormId(index);
}

XFormObject* XFormBufferManager::Get(unsigned int index)
{
    return &mWriteBuffer->buffer[index];
}

XFormBuffer* XFormBufferManager::GetReadBuffer() const
{
    return mReadBuffer;
}

bool XFormBufferManager::ReadBufferDirty() const
{
    return mReadBufferDirty;
}

void XFormBufferManager::SwapReadBuffers()
{
    // Swap buffers, volatile read should ensure memory ordering of read is consistent (MSVC only, must use read barrier normally)
    XFormBuffer* volatile temp = mPivotBuffer;

    // Pivot write needs to be made available on other thread immediately
    InterlockedExchangePointer(&mPivotBuffer, mReadBuffer);

    // This write only matters to render thread
    mReadBuffer = (XFormBuffer*)temp;
    
    mReadBufferDirty = false;
}

void XFormBufferManager::SwapWriteBuffers()
{
    // Swap buffers, volatile read should ensure memory ordering of read is consistent (MSVC only, must use read barrier normally)
    XFormBuffer* volatile temp = mPivotBuffer;

    // Pivot write needs to be made available on other thread immediately
    InterlockedExchangePointer(&mPivotBuffer, mWriteBuffer);

    // This write only matters to main thread
    mWriteBuffer = (XFormBuffer*)temp;

    mReadBufferDirty = true;

    // Essentially all we're doing is copying the results of main thread processing last frame
    // into our new buffer before using it for this new frame (Copy buffer + size)
    memcpy(mWriteBuffer, (XFormBuffer*)mPivotBuffer, sizeof(XFormObject) * mPivotBuffer->size + sizeof(unsigned int));
}

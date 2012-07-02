#pragma once

#include "Vec2.h"

class GameObject;

// Data that needs to be read by graphics and modified by logic
struct XFormObject
{
    Vec2 pos;
    float scale;
    float colorMod; // Modifier value for color (Anger for boids, damage for players)
    GameObject* owner; // Used to update xform index/call draw logic
};

struct XFormBufferHandle
{
    XFormObject* bufferPtr;
    unsigned int bufferSize;
};

class XFormBuffer
{
public:

    XFormBuffer();
    ~XFormBuffer();

    // Called by RenderThread
    XFormBufferHandle SyncBuffer();

    // Called by main thread
    void CopyBuffer();

    void Add(GameObject* object); // Call when objects are created on main thread
    void Remove(unsigned int index); // Call when objects are destroyed on main thread
    XFormObject* Get(unsigned int index); // Retrieves xform data from buffer

private:

    // This is the total number of objects you will ever have in your game.
    // If you notice your game needs more xforms, increase this value! Much more
    // efficient than suffering a costly reallocation penalty every time you hit capacity
    static const unsigned int BUFFER_CAPACITY = 128;

    void* mRenderCompleteEvent;
    void* mBufferSwapEvent;

    // Two buffers, A is read by render thread, B is written/read by main thread
    // At end of frame we do a sync where we copy the contents of buffer B into A
    XFormObject* mBufferPtrA;
    XFormObject* mBufferPtrB;

    XFormObject mBufferA[BUFFER_CAPACITY];
    XFormObject mBufferB[BUFFER_CAPACITY];

    // Buffer size as modified by main thread when adding new objects
    unsigned int mActualBufferSize;

    // Buffer size copy as read by RenderThread
    unsigned int mDirtyBufferSize;

};

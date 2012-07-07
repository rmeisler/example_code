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

// This is the total number of objects you will ever have in your game.
// If you notice your game needs more xforms, increase this value! Much more
// efficient than suffering a costly reallocation penalty every time you hit capacity
static const unsigned int BUFFER_CAPACITY = 128;

struct XFormBuffer
{
    unsigned int size;
    XFormObject buffer[BUFFER_CAPACITY];
};

// This is basically a specialized version of a slot map
class XFormBufferManager
{
public:

    XFormBufferManager();

    // Called by RenderThread
    void SwapReadBuffers();
    XFormBuffer* GetReadBuffer() const;

    // Only wanna swap read buffers when dirty, this allows us to safely
    // cleanup objects after a swap without fear that dead objects will return
    // on a stale swap (i.e. swap with buffer that main thread hasnt yet modified)
    bool ReadBufferDirty() const;

    // Called by main thread
    void SwapWriteBuffers();

    void Add(GameObject* object); // Call when objects are created on main thread
    void Remove(unsigned int index); // Call when objects are destroyed on main thread
    XFormObject* Get(unsigned int index); // Retrieves xform data from buffer

private:

    // Triple buffering, requires no syncing
    // BufferPtrA read by main thread, BufferPtrB is our pivot, BufferPtrC read by render thread
    XFormBuffer* mBufferPtrA;
    XFormBuffer* mBufferPtrB;
    XFormBuffer* mBufferPtrC;

    XFormBuffer mBufferA;
    XFormBuffer mBufferB;
    XFormBuffer mBufferC;

    bool mReadBufferDirty;

};

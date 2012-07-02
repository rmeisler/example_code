#pragma once

// Inherit from this to add new render thread messages
class Message
{
public:
    Message(int type) : type(type) {}
    int type;
};

class RenderThread
{
public:

    RenderThread(int argc, char** argv);
    ~RenderThread();

    void SendMessage(Message* msg);
    void HandleMessages();

    void Render();
    void Sync();

private:

    static const unsigned int NUM_MESSAGES_PER_LOOP = 10;
    static unsigned long __stdcall ThreadFunction(void* data);

    // Buffer of xform data, copied from main thread xform buffer once per frame
    XFormBufferHandle mBufferHandle;

    // Message queue for asynchronous communication with render thread
    ConcurrentQueue<Message> mQueue;

    void* mThreadHnd;
    volatile bool mRunning;

    // Command line args, needed for glut
    int mArgC;
    char** mArgV;

};

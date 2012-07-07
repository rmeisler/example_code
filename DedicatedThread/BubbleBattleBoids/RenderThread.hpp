#pragma once

// Inherit from this to add new render thread messages
class Message
{
public:
    Message(int type) : type(type) {}
    virtual ~Message() {}
    int type;
};

class KillObjectMsg : public Message
{
public:
    static const unsigned id = 0;
    KillObjectMsg(GameObject* object) : Message(0), object(object) {}
    GameObject* object;
};

class RenderThread
{
public:

    RenderThread(int argc, char** argv);
    ~RenderThread();

    void Send(Message* msg);
    void HandleMessages();

    void CleanUp();
    void Render();

private:

    static const unsigned int NUM_MESSAGES_PER_LOOP = 10;
    static unsigned long __stdcall ThreadFunction(void* data);

    // Message queue for asynchronous communication with render thread
    ConcurrentQueue<Message> mQueue;

    // Dead objects are sent here by main thread for render thread to clean up on its own time
    std::vector<GameObject*> mDeadObjects;
      
    void* mThreadHnd;
    volatile bool mRunning;

    // Command line args, needed for glut
    int mArgC;
    char** mArgV;

};

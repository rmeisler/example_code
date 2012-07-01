#pragma once

// Inherit from this to add new render messages
class Message
{
public:
    Message(int type) : type(type) {}
    int type;
};

enum RenderMessages
{
    ADD_OBJECT_MSG,
    REMOVE_OBJECT_MSG
};

class GameObject;

class AddObjectMsg : public Message
{
public:
    AddObjectMsg(GameObject* obj) : Message(ADD_OBJECT_MSG), obj(obj) {}
    GameObject* obj;
};

class RemoveObjectMsg : public Message
{
public:
    RemoveObjectMsg(GameObject* obj) : Message(REMOVE_OBJECT_MSG), obj(obj) {}
    GameObject* obj;
};

class RenderThread
{
public:

    RenderThread(int argc, char** argv);
    ~RenderThread();

    void SendMessage(Message* msg);
    void Render();
    void HandleMessages();

private:

    static unsigned long __stdcall ThreadFunction(void* data);

    std::vector<GameObject*> mRenderObjects;

    ConcurrentQueue<Message> mQueue;

    void* mThreadHnd;
    volatile bool mRunning;

    // Command line args, needed for glut
    int mArgC;
    char** mArgV;

};

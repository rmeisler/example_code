#pragma once

#include "ConcurrentQueue.hpp"

class Message;

// If you aren't dedicating the main thread to message processing
// you will probably want to communicate to objects on the
// main thread from your actor(s). Inheriting from MailBox allows
// you to receive messages from actors as if you were one (using Send).
// You can then process any messages you've received on your own
// time (using GetNextMessage). Likely this will happen in an Update
// function that's called once per frame
class MailBox
{
public:

    void Send(Message* msg);
    Message* GetNextMessage();

private:

    ConcurrentQueue<Message> mMessages;

};

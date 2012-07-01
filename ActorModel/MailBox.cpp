#include "MailBox.hpp"
#include "Actor.hpp"

void MailBox::Send(Message* msg)
{
    mMessages.push(msg);
}

Message* MailBox::GetNextMessage()
{
    Message* msg = NULL;
    return mMessages.try_pop(msg) ? msg : NULL;
}

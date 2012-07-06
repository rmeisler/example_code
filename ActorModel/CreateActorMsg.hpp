#pragma once

#include "Actor.hpp"

class CreateActorMsg : public Message
{
public:
    static const unsigned id = -1;

    CreateActorMsg() : Message(id) {}
};

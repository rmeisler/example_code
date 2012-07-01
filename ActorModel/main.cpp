/////////////////////////////////////////////////////////////////////
// The following code approximates PI using both serial and parallel
// implementations. PI approximation is an "embarrasingly parallel"
// algorithm, why is why I'm using it to demonstrate the use of a
// Actor Model system. Though this is fairly contrived, it's not
// unlike how you might use actors for physics broadphase/narrowphase.
// 
// This is primarily to demonstrate how actors can be used to
// accomplish the same things that tasks can.
/////////////////////////////////////////////////////////////////////
#include "Actor.hpp"
#include "Scheduler.hpp"
#include "ActorFactory.hpp"
#include "AlignedAlloc.hpp"

#include <stdio.h>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

void InitTimer();
double GetTime();

class PiResultMsg : public Message
{
public:

    static const unsigned id = 0;

    PiResultMsg(double acc) : Message(id), acc(acc) {}
    double acc;
};

class PiRecvActor : public Actor
{
public:
  
  PiRecvActor() : totalPiAcc(0.0), numMessagesReceived(0) {}
  
  virtual void HandleMessage(Message* msg)
  {
    double acc = ((PiResultMsg*)msg)->acc;
    totalPiAcc += acc;
    
    if( ++numMessagesReceived == 40 )
    {
      double result = totalPiAcc;
      printf("Pi = %f\nThis took %f milliseconds!\n"
             "Try running this multiple times, if a"
             " thread is scheduled onto a new core "
             "your timestamp will look overly large", result, GetTime());

      Stop();
    }
  }
  
private:
  
  double totalPiAcc;
  int numMessagesReceived;
  
};

class PiWorkerActor : public Actor
{
public:
  
  PiWorkerActor(ActorHandle recv, int start, int end) : recv(recv), start(start), end(end) {}
  
  virtual void HandleMessage(Message* msg)
  {
    double acc = 0.0;
    for( int i = start; i < end; i++ )
    {
      acc += 4.0 * (1.0 - (i % 2) * 2) / (2 * i + 1);
    }
    
    Actor* recvActor = recv.Get();
    if( recvActor )
        recvActor->Send(AlignedAlloc<PiResultMsg>(acc));

    // Destroys actor after handler returns
    Stop();
  }
  
  ActorHandle recv;
  int start;
  int end;
  
};

void Parallel()
{
    auto actorRecv = gActorFactory->Create<PiRecvActor>();
    
    // Can tweak with actor load for perf
    for( int i = 0; i < 40; i++ )
    {
        auto actor = gActorFactory->Create<PiWorkerActor>(actorRecv, i * 1000000, (i + 1) * 1000000 - 1);

        // Sends message then immediately stops actor
        actor.Get()->Spawn();
    }

    gScheduler->DedicateMainThread();
}

void Serial()
{
    double acc = 0.0;
    int start = 0, end = 40000000;
    for( int i = start; i < end; i++ )
    {
        acc += 4.0 * (1.0 - (i % 2) * 2) / (2 * i + 1);
    }

    printf("Pi = %f\nThis took %f milliseconds!\n"
           "Try running this multiple times, if a"
           " thread is scheduled onto a new core "
           "your timestamp will look overly large", acc, GetTime());
}

void main()
{
    InitTimer();

    Parallel();

    system("pause");
}

/////////////////////////////////////////////////////////////////////
// The following code approximates PI using both serial and parallel
// implementations. PI approximation is an "embarrasingly parallel"
// algorithm, why is why I'm using it to demonstrate the use of a
// Actor Model system. Though this is fairly contrived, it's not
// unlike how you might use actors for physics broadphase/narrowphase.
// 
// This is primarily to demonstrate how actors and message passing
// might work.
/////////////////////////////////////////////////////////////////////
#include "Actor.hpp"
#include "Scheduler.hpp"
#include "ActorFactory.hpp"

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
             "Try running this multiple times to get an average time", result, GetTime());

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
    
    recv.Send(new PiResultMsg(acc));
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
        auto actor = gActorFactory->Create<PiWorkerActor>(actorRecv, i * 1000000, (i + 1) * 1000000);

        // Sends message then immediately stops actor
        actor.Send(new Message(0));
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
           "Try running this multiple times to get an average time", acc, GetTime());
}

void main()
{
    InitTimer();

    Parallel();

    system("pause");
}

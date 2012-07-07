#include "PCH.h"
#include "RenderThread.hpp"

// BBB includes
#include "Player.h"
#include "Graphics.h"
#include "GameObject.h"
#include "Vec2.h"

// BBB state
enum State
{
	StateStart,
	StatePlay,
	StateDone
};

// BBB Globals
extern State g_State;
extern double g_TotalTime;
extern Player::PlayerNum g_Winner;
extern int g_NumPlayers;

// RenderThread globals
RenderThread* g_RenderThread = NULL;
extern XFormBufferManager* g_XFormBuffer;

// Use this to test timing
void InitTimer();
void StartTime();
double GetTime();

RenderThread::RenderThread(int argc, char** argv) : mRunning(true), mArgC(argc), mArgV(argv)
{
    mThreadHnd = CreateThread(0, 0, ThreadFunction, 0, 0, 0);
}

RenderThread::~RenderThread()
{
    // Cleanup thread
    mRunning = false;
    
    WaitForSingleObject(mThreadHnd, 1000);
    CloseHandle(mThreadHnd);
}

void RenderThread::Send(Message* msg)
{
    mQueue.push(msg);
}

void RenderThread::HandleMessages()
{
    Message* msg = NULL;
    unsigned int messagesHandled = 0;
    
    while( mQueue.try_pop(msg) )
    {
        // Handle any messages we might have for render thread...
        switch(msg->type)
        {
            case KillObjectMsg::id:
                mDeadObjects.push_back(((KillObjectMsg*)msg)->object);
                break;
        }

        delete msg;

        // Handle some set number of messages per loop, don't want to be overwhelmed by message
        // processing and forget about actually rendering the scene!
        if( messagesHandled++ < NUM_MESSAGES_PER_LOOP )
            break;
    }
}

void RenderThread::Render()
{
    XFormBuffer* bufferHandle = g_XFormBuffer->GetReadBuffer();
    
    // We don't really have any special culling logic or ordering, so we can just walk the xform buffer and draw each object
    for( unsigned int i = 0; i < bufferHandle->size; i++ )
    {
        XFormObject* obj = &bufferHandle->buffer[i];

		    glPushMatrix();
		    glTranslatef(obj->pos.x, obj->pos.y, 0.f);

        obj->owner->Draw(obj);

		    glPopMatrix();
	  }
}

void RenderThread::CleanUp()
{
    // Destroy dead objects
    while(!mDeadObjects.empty())
    {
        delete mDeadObjects.back();
        mDeadObjects.pop_back();
    }
}

void Display();

DWORD WINAPI RenderThread::ThreadFunction(void* data)
{
    // Initialize GLUT
	glutInit(&g_RenderThread->mArgC, g_RenderThread->mArgV);
	glutInitWindowSize(800, 600);
	glutInitWindowPosition(100, 100);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutCreateWindow("Bad-ass Bubble Boid Battle Bots Bonanza");

	glutDisplayFunc(&Display);

    // Run graphics loop
	glutMainLoop();

  return 0;
}

// Render the scene
void Display()
{
  StartTime();
    
	// Initialize the rendering
	glViewport(0, 0, 800, 600);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, 20, 0, 15, 0, 1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Clear the screen
	glClearColor(0.5f, 0.5f, 0.5f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT);

	// Draw all game objects
	g_RenderThread->Render();

	// Screen mode
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, 800, 0, 600, 0, 1);

	// Timer
	char timer[64];
	sprintf_s(timer, "%02.2f", g_TotalTime);
	DrawText(Vec2(710.f, 585.f), .1, timer);

	// Draw state text
	switch (g_State)
	{
	case StateStart:
		DrawText(Vec2(130.f, 300.f), .3f, "Press (A) to Join");
		if (g_NumPlayers > 0)
			DrawText(Vec2(110.f, 250.f), .3f, "Press Start to Play");
		break;
	case StatePlay:
		break;
	case StateDone:
		DrawText(Vec2(270.f, 300.f), .3f, "Game Over");
		switch (g_Winner)
		{
		case Player::NNobody:
			break;
		case Player::NOne:
			DrawText(Vec2(270.f, 250.f), .2f, "Player One Wins!");
			break;
		case Player::NTwo:
			DrawText(Vec2(270.f, 250.f), .2f, "Player Two Wins!");
			break;
		case Player::NThree:
			DrawText(Vec2(270.f, 250.f), .2f, "Player Three Wins!");
			break;
		case Player::NFour:
			DrawText(Vec2(270.f, 250.f), .2f, "Player Four Wins!");
			break;
		}
		break;
	}

	// Swap buffers
	glutSwapBuffers();

  // Handle messages
  g_RenderThread->HandleMessages();

  // Read from other buffer
  if( g_XFormBuffer->ReadBufferDirty() )
  {
    g_XFormBuffer->SwapReadBuffers();
    g_RenderThread->CleanUp();
  }

  // Redraw immediately
	glutPostRedisplay();
}

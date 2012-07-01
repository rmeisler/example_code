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
static unsigned int NUM_MESSAGES_PER_LOOP = 10;
RenderThread* g_RenderThread = NULL;

RenderThread::RenderThread(int argc, char** argv) : mRunning(true), mArgC(argc), mArgV(argv)
{
    mThreadHnd = CreateThread(0, 0, ThreadFunction, 0, 0, 0);
}

RenderThread::~RenderThread()
{
    mRunning = false;

    WaitForSingleObject(mThreadHnd, 1000);
    CloseHandle(mThreadHnd);
}

void RenderThread::SendMessage(Message* msg)
{
    mQueue.push(msg);
}

void RenderThread::Render()
{
    // Since render thread never really writes to object parameters, it only reads them,
    // we can simply let GameObjects act as read-only shared data and not worry about
    // any synchronization issues

    // Note that we keep a separate container of GameObjects on the render thread, aside from the
    // intrusive list that the main thread uses. This is so we can unlink the objects from the main
    // thread while maintaining references on the render thread, such that each thread can remove
    // the object when they are good and ready
    for( auto itr = mRenderObjects.begin(); itr != mRenderObjects.end(); itr++ )
    {
		glPushMatrix();
		glTranslatef((*itr)->GetPosition().x, (*itr)->GetPosition().y, 0.f);
		(*itr)->Draw();
		glPopMatrix();
	}
}

void RenderThread::HandleMessages()
{
    Message* msg = NULL;
    unsigned int messagesHandled = 0;

    // Handle some set number of messages per loop, don't want to be overwhelmed by message
    // processing and forget about actually rendering the scene!
    while( messagesHandled++ < NUM_MESSAGES_PER_LOOP &&
           mQueue.try_pop(msg) )
    {
        switch(msg->type)
        {
            case ADD_OBJECT_MSG:
                mRenderObjects.push_back(((AddObjectMsg*)msg)->obj);
            break;

            case REMOVE_OBJECT_MSG:
                GameObject* obj = ((RemoveObjectMsg*)msg)->obj;

                // Find object and remove it
                for( auto itr = mRenderObjects.begin(); itr != mRenderObjects.end(); itr++ )
                {
                    if( obj == *itr )
                    {
                        // For destruction of objects to happen safely, the main thread
                        // only ever unlinks objects. Render thread destroys them
                        mRenderObjects.erase(itr);
                        delete obj;
                        break;
                    }
                }
            break;
        }
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

    // Redraw immediately
	glutPostRedisplay();
}

#include "PCH.h"
#include "GameObject.h"
#include "Game.h"

GameObject* GameObject::s_Head = NULL;
GameObject* GameObject::s_Tail = NULL;

GameObject::GameObject(const Vec2& position) : m_Damping(0.1f), m_Mass(1.f), m_Next(NULL), m_Prev(NULL), m_Shape(Circle), m_Dead(false)
{
    // TODO: Not all objects need this data sync'd since it will never really change on static objects like walls or pits,
    // this could be a nice, simple optimization
    g_XFormBuffer->Add(this);

    SetPosition(position);
    SetScale(1.0f);

	if (s_Head == NULL)
	{
		s_Head = s_Tail = this;
	}
	else
	{
		s_Tail->m_Next = this;
		m_Prev = s_Tail;
		s_Tail = this;
	}
}

void GameObject::Unlink()
{
    if (m_Next != NULL)
		m_Next->m_Prev = m_Prev;
	if (m_Prev != NULL)
		m_Prev->m_Next = m_Next;
	if (s_Head == this)
		s_Head = m_Next;
	if (s_Tail == this)
		s_Tail = m_Prev;
}

void GameObject::IntegratePhysics()
{
    XFormObject* obj = g_XFormBuffer->Get(m_XFormId);

    // integrate physics
	Vec2 accel = m_Force / m_Mass;
	Vec2 damping = -m_Damping * m_Velocity;
	m_Velocity = m_Velocity + (accel + damping) * g_FrameTime;
	m_Velocity = m_Velocity.Clamp(20.f);
	m_PositionPrev = obj->pos;
	obj->pos = obj->pos + m_Velocity * g_FrameTime;

	// clear force
	m_Force = Vec2();
}

void GameObject::RemoveFromSharedData()
{
    GameObject* obj = s_Head;
	while (obj != NULL)
	{
        // Remove dead objects from xform buffer
		if (obj->m_Dead)
            g_XFormBuffer->Remove(obj->m_XFormId);

		obj = obj->m_Next;
	}
}

void GameObject::CleanUp()
{
	GameObject* obj = s_Head;
	while (obj != NULL)
	{
		if (obj->m_Dead)
		{
			GameObject* next = obj->m_Next;
            obj->Unlink();

            delete obj;
			
            obj = next;
		}
		else
			obj = obj->m_Next;
	}
}
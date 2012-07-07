#pragma once

#include "Vec2.h"

extern XFormBufferManager* g_XFormBuffer;

// Base game object
class GameObject
{
public:
	GameObject(const Vec2& position);
    virtual ~GameObject() {}

	enum Type
	{
		TPlayer,
		TBoid,
		TPit,
		TWall
	};

	enum Shape
	{
		Circle,
		Rectangle
	};

    void Unlink();

	virtual Type GetType() const = 0;

	Shape GetShape() const { return m_Shape; }
	void SetShape(Shape shape) { m_Shape = shape; }

	bool IsDead() const { return m_Dead; }
	void Destroy() { m_Dead = true; }

	const Vec2& GetPosition() const { return g_XFormBuffer->Get(m_XFormId)->pos; }
	void SetPosition(const Vec2& position)
    {
        XFormObject* obj = g_XFormBuffer->Get(m_XFormId);
        obj->pos = m_PositionPrev = position;
    }
	const Vec2& GetPositionPrev() const { return m_PositionPrev; }

	float GetScale() const { return g_XFormBuffer->Get(m_XFormId)->scale; }
	void SetScale(float scale)
    {
        XFormObject* obj = g_XFormBuffer->Get(m_XFormId);
        obj->scale = scale;
    }

	const Vec2& GetVelocity() const { return m_Velocity; }
	void SetVelocity(const Vec2& velocity) { m_Velocity = velocity; }

	const Vec2& GetForce() const { return m_Force; }
	void AddForce(const Vec2& force) { m_Force = m_Force + force; }

	float GetDamping() const { return m_Damping; }
	void SetDamping(float damping) { m_Damping = damping; }

	float GetMass() const { return m_Mass; }
	void SetMass(float mass) { m_Mass = mass; }

    // Set id for accessing shared render data
    unsigned int GetXFormId() const { return m_XFormId; }
    void SetXFormId(unsigned int id) { m_XFormId = id; }

	virtual void PreUpdate() = 0;
	virtual void PostUpdate() = 0;
	virtual void Draw(XFormObject*) = 0;
	virtual void Collision(GameObject* other, const Vec2& point, const Vec2& normal) = 0;

	void IntegratePhysics();

	GameObject* GetNext() const { return m_Next; }

	static GameObject* All() { return s_Head; }
	static void CleanUp();
    static void RemoveFromSharedData();

private:
	Vec2 m_PositionPrev;
	Vec2 m_Velocity;
	Vec2 m_Force;
	float m_Damping;
	float m_Mass;

    unsigned int m_XFormId;

	GameObject* m_Next;
	GameObject* m_Prev;

	Shape m_Shape;

	bool m_Dead;

	static GameObject* s_Head;
	static GameObject* s_Tail;
};
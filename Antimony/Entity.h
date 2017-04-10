#pragma once

#include "DirectX.h"
#include "Bullet.h"

///

class Entity
{
protected:
	bool m_free;
	float m_gravity;

public:
	void lock();
	void unlock();
	bool isFree();

	Entity();
};

class SimpleEntity : public Entity
{
protected:
	float3 m_pos, m_posDest;
	long double m_posResponse;
	float m_fallingSpeed;

public:
	void updatePos(double delta);
	void moveToPoint(float3 dest, long double r);
	void setGravity(float g);

	float3 getPos();
	float3 getPosDest();

	SimpleEntity();
};

class LookAtEntity
{
protected:
	float3 m_lookat, m_lookatDest;
	long double m_lookatResponse;

public:
	void updateLookAt(double delta);
	void lookAtPoint(float3 dest, long double r);

	float3 getLookAt();
	float3 getLookAtDest();

	LookAtEntity();
};

// (UNUSED)
class PhysEntity
{
protected:
	btDiscreteDynamicsWorld *m_world;
	mat m_initialTransform;

public:
	btObject *m_coll; // temp; for debugging

	void reset();

	PhysEntity(LPCWSTR name, float mass, btCollisionShape *cs, btDefaultMotionState *ms, btDiscreteDynamicsWorld *w = btWorld);
	PhysEntity(LPCWSTR name, float mass, btCollisionShape *cs, btDefaultMotionState *ms, btVector3 *in, btDiscreteDynamicsWorld *w = btWorld);
	~PhysEntity();
};

// (UNUSED)
class CharEntity
{
protected:
	btDiscreteDynamicsWorld *m_world;

public:
	btPairCachingGhostObject *gh;
	btConvexShape *xs;
	btKinematicCharacterController *cc;

	mat getTransform();
	void setTransform(mat *m);
	void step(double delta);

	CharEntity(LPCWSTR name, btPairCachingGhostObject *g, btConvexShape *s, btDiscreteDynamicsWorld *w = btWorld);
	~CharEntity();
};
#pragma once

#include "Entity.h"

///

// (UNUSED)
class SimpleCharacter : public SimpleEntity, public LookAtEntity
{
protected:
	PhysEntity *coll;
	CharEntity *cont;

public:
	char jumping;
	char action;
	float mov_speed;
	float jump_speed;

	void update(double delta);
	void updatePhysics(float3 p, float3 d);
	void jump();

	SimpleCharacter();
};

class Character : public LookAtEntity
{
protected:
	btObject *m_collisionObject;

public:
	bool moving;
	char jumping;
	char action;
	// TODO: 3D character asset
	float mov_speed;
	float jump_speed;

	btVector3 lin_worldvel;
	btVector3 ang_worldvel;

	//btVector3 rvel;

	void update(double delta);
	void warp(Vector3 d);
	void move(btVector3 *v, float s);
	void attemptJump();

	void setCollisionObject(btObject *pc);
	btObject* getColl();

	Vector3 getPos();

	Character();
	~Character();
};
#pragma once

#include "Entity.h"

///

#define JUMPSTATE_ONGROUND	0x00
#define JUMPSTATE_JUMPING	0x01
#define JUMPSTATE_INAIR		0x02
#define JUMPSTATE_LANDING	0x03

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

	bool m_moving;
	char m_jumping;
	char m_action;
	// TODO: 3D character asset

public:
	float m_movSpeed;
	float m_jumpSpeed;
	btVector3 m_linWorldVel;
	btVector3 m_angWorldVel;

	//btVector3 rvel;

	void update(double delta, std::map<const btCollisionObject*, std::vector<btPersistentManifold*>> objectsCollisions);
	void updateContact(char contact);
	void warp(Vector3 d);
	void move(btVector3 *v, float s);
	void attemptJump();

	void setCollisionObject(btObject *pc);
	btObject* getColl();

	Vector3 getPos();
	bool getMovingState();
	char getJumpState();
	char getActionState();

	Character();
	~Character();
};
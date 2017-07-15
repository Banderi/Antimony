#pragma once

#include "Entity.h"

///

#define CAM_THETA 0
#define CAM_PHI 1

///

class Camera : public SimpleEntity, public LookAtEntity // TODO: implement camera collisions
{
protected:
	btObject *m_collisionObject;
	btGeneric6DofConstraint *m_collisionConstraint;

	float m_friction;
	float m_th, m_thVel;
	float m_ph, m_phVel;
	float m_quake;

public:
	float zoom;
	float displacement;

	void update(double delta);

	void rotate(bool angle, float acceleration);
	void setAngles(float t, float p);
	float getAngle(bool angle);

	void setFriction(float f);
	float getFriction();

	void quake(float t, float q); // TODO: implement quake

	void setCollisionObject(btObject *pc);
	btObject* getColl();

	void setCollisionConstraint(btGeneric6DofConstraint *pc);
	btGeneric6DofConstraint* getConstr();

	Camera();
};
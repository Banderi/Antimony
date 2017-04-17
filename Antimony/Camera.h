#pragma once

#include "Entity.h"

///

#define CAM_THETA 0
#define CAM_PHI 1

///

class Camera : public SimpleEntity, public LookAtEntity // TODO: implement camera collisions
{
protected:
	float m_friction;
	float m_th, m_thVel;
	float m_ph, m_phVel;
	float m_quake;

public:
	void update(double delta);

	void rotate(bool angle, float acceleration);
	void setAngles(float t, float p);
	float getAngle(bool angle);

	void setFriction(float f);
	float getFriction();

	void addQuake(float q); // TODO: implement quake

	Camera();
};
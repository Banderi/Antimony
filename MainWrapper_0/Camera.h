#ifndef CAMERA_H
#define CAMERA_H

#include "Entity.h"

#define theta 0
#define phi 1

class Camera : public Entity
{
	float friction;
	float th, th_vel;
	float ph, ph_vel;
	float quake;

public:
	void update(float delta);

	void rotate(bool angle, float acceleration);
	void setAngles(float t, float p);
	float getAngle(bool angle);

	void setFriction(float f);
	float getFriction();

	void addQuake(float q);

	Camera();
};

extern Camera camera;

#endif
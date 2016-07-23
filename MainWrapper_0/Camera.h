#ifndef CAMERA_H
#define CAMERA_H

#include "DirectX11.h"

#define theta 0
#define phi 1

class Camera
{
	bool free;
	vec3 pos, pos_dest;
	vec3 lookat, lookat_dest;
	float response;
	float friction;
	float th, th_vel;
	float ph, ph_vel;
	float quake;

public:
	void update(float delta);
	
	void lock();
	void unlock();
	bool isfree();

	void moveToPoint(vec3 dest, float r);
	void lookAtPoint(vec3 dest, float r);

	void rotate(bool angle, float acceleration);
	void setAngles(float t, float p);
	float getAngle(bool angle);

	void setFriction(float f);
	float getFriction();

	void addQuake(float q);

	vec3 getPos();
	vec3 getLookAt();	

	Camera();
};

extern Camera camera;

#endif
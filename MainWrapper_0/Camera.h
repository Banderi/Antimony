#ifndef CAMERA_H
#define CAMERA_H

#include "DirectX11.h"

#define th 0
#define ph 1

class Camera
{
	vec3 pos;
	vec3 lookat;
	float theta;
	float phi;

public:
	void moveToPoint(vec3 dest, float response);
	void lookAtPoint(vec3 dest, float response);
	void setAngle(float t, float p);
	vec3 getPos();
	vec3 getLookAt();
	float getAngle(bool a);

	Camera();
};

extern Camera camera;

#endif
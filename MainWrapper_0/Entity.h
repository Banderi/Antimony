#ifndef ENTITY_H
#define ENTITY_H

#include "DirectX11.h"

class Entity
{
protected:
	bool free;
	vec3 pos, pos_dest;
	vec3 lookat, lookat_dest;
	float response;

public:
	void lock();
	void unlock();
	bool isfree();

	void moveToPoint(vec3 dest, float r);
	void lookAtPoint(vec3 dest, float r);

	vec3 getPos();
	vec3 getLookAt();
};

#endif

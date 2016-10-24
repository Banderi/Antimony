#ifndef ENTITY_H
#define ENTITY_H

#include "DirectX.h"

class Entity
{
protected:
	bool free;
	float3 pos, pos_dest;
	float3 lookat, lookat_dest;
	float response;

public:
	void updatePos(double delta);

	void lock();
	void unlock();
	bool isfree();

	void moveToPoint(float3 dest, double r);
	void lookAtPoint(float3 dest, double r);

	float3 getPos();
	float3 getPosDest();
	float3 getLookAt();
	float3 getLookAtDest();
};

#endif

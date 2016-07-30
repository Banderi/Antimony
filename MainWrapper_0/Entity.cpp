#include "Entity.h"

void Entity::updatePos(float delta)
{
	float mult = response;

	float3 v = pos_dest - pos;

	if (((v.Length() * mult * delta) <= v.Length()) && !((v * mult * delta).Length() <= v.Length()))
		int mah = 1;

	if (1)
	{
		float3 v = pos_dest - pos;
		if ((v.Length() * mult * delta) <= v.Length())
			pos += v * mult * delta;
		else
			pos = pos_dest;
		v = lookat_dest - lookat;
		if ((v.Length() * mult * delta) <= v.Length())
			lookat += v * mult * delta;
		else
			lookat = lookat_dest;
	}
	else if (1)
	{
		float3 v = pos_dest - pos;
		if ((v * mult * delta).Length() <= v.Length())
			pos += v * mult * delta;
		else
			pos = pos_dest;
		v = lookat_dest - lookat;
		if ((v * mult * delta).Length() <= v.Length())
			lookat += v * mult * delta;
		else
			lookat = lookat_dest;
	}
	else
	{
		pos = pos_dest;
		lookat = lookat_dest;
	}

	if (pos == pos_dest)
		response = 0.0f;
	if (lookat == lookat_dest)
		response = 0.0f;
}
void Entity::lock()
{
	free = false;
}
void Entity::unlock()
{
	free = true;
}
bool Entity::isfree()
{
	return free;
}
void Entity::moveToPoint(float3 dest, float r)
{
	if (r != -1)
	{
		response = r;
		pos_dest = dest;
	}
	else
	{
		pos = dest;
		pos_dest = dest;
	}		
}
void Entity::lookAtPoint(float3 dest, float r)
{
	if (r != -1)
	{
		response = r;
		lookat_dest = dest;
	}
	else
	{
		lookat = dest;
		lookat_dest = dest;
	}
}
float3 Entity::getPos()
{
	return pos;
}
float3 Entity::getPosDest()
{
	return pos_dest;
}
float3 Entity::getLookAt()
{
	return lookat;
}
float3 Entity::getLookAtDest()
{
	return lookat_dest;
}
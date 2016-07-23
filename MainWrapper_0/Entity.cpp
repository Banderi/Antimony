#include "Entity.h"


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
void Entity::moveToPoint(vec3 dest, float r)
{
	if (r != -1)
	{
		response = r;
		pos_dest = dest;
	}
	else
		pos = dest;
}
void Entity::lookAtPoint(vec3 dest, float r)
{
	if (r != -1)
	{
		response = r;
		lookat_dest = dest;
	}
	else
		lookat = dest;
}
vec3 Entity::getPos()
{
	return pos;
}
vec3 Entity::getLookAt()
{
	return lookat;
}
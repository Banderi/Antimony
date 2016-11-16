#include "Entity.h"

#define clamp(x, lower, upper) (min(upper, max(x, lower)))

void Entity::updatePos(double delta)
{
	float3 mov;

	long double mult;
	long double base;
	long double response;
	long double exponent;

	response = pos_response;
	base = 1.0f - response;
	exponent = delta / response;
	mult = 1.0f - pow(base, exponent);

	mov = pos_dest - pos;
	pos += mov * (float)mult;

	response = lookat_response;
	base = 1.0f - response;
	exponent = delta / response;
	mult = 1.0f - pow(base, exponent);

	mov = lookat_dest - lookat;
	lookat += mov * (float)mult;

	/*if (pos == pos_dest)
		pos_response = 0;
	if (lookat == lookat_dest)
		lookat_response = 0;*/
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
void Entity::moveToPoint(float3 dest, long double r)
{
	pos_dest = dest;
	if (r != -1)
	{
		if (r < 0)
			r = 0;
		else if (r > 1)
			r = 1;
		pos_response = r;
	}
	else
		pos_response = 1;
}
void Entity::lookAtPoint(float3 dest, long double r)
{
	lookat_dest = dest;
	if (r != -1)
	{
		if (r < 0)
			r = 0;
		else if (r > 1)
			r = 1;
		lookat_response = r;
	}
	else
		lookat_response = 1;
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
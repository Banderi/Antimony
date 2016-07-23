#include "Character.h"
#include "DebugWin.h"

void Character::update(float delta)
{
	pos -= (pos - pos_dest) * response * 75 * delta;
	lookat -= (lookat - lookat_dest) * response * 75 * delta;

	if (pos == pos_dest)
		response = 0.0f;
	if (lookat == lookat_dest)
		response = 0.0f;
}
void Character::lock()
{
	free = false;
}
void Character::unlock()
{
	free = true;
}
bool Character::isfree()
{
	return free;
}
void Character::moveToPoint(vec3 dest, float r)
{
	if (r != -1)
	{
		response = r;
		pos_dest = dest;
	}
	else
		pos = dest;
}
void Character::lookAtPoint(vec3 dest, float r)
{
	if (r != -1)
	{
		response = r;
		lookat_dest = dest;
	}
	else
		lookat = dest;
}
vec3 Character::getPos()
{
	return pos;
}
vec3 Character::getLookAt()
{
	return lookat;
}

Character::Character()
{
	free = true;
	pos = vec3(0, 0, 0);
	pos_dest = vec3(0, 0, 0);
	lookat = vec3(0, 0, 0);
	lookat_dest = vec3(0, 0, 0);
	response = 0.0f;
}
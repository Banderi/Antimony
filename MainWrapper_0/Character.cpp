#include "Character.h"
#include "DebugWin.h"

void Character::moveToPoint(vec3 dest, float response)
{
	if (response != -1)
		pos -= (pos - dest) * response;
	else
		pos = dest;
}
void Character::lookAtPoint(vec3 dest, float response)
{
	if (response != -1)
		lookat -= (lookat - dest) * response;
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
	pos = vec3(0, 0, 0);
	lookat = vec3(0, 0, 0);
}
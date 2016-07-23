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

Character::Character()
{
	free = true;
	pos = vec3(0, 0, 0);
	pos_dest = vec3(0, 0, 0);
	lookat = vec3(0, 0, 0);
	lookat_dest = vec3(0, 0, 0);
	response = 0.0f;
}
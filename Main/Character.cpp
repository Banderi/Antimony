#include "Character.h"
#include "DebugWin.h"

void Character::update(double delta)
{
	updatePos(delta);
}

Character::Character()
{
	free = true;
	pos = float3(0, 0, 0);
	pos_dest = float3(0, 0, 0);
	lookat = float3(0, 0, 0);
	lookat_dest = float3(0, 0, 0);
	pos_response = 0;
	lookat_response = 0;
}
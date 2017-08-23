#include "Gameplay.h"

///

Player Antimony::player;
Camera Antimony::camera_main;

Player* Antimony::getPlayer()
{
	return &player;
}
Camera* Antimony::getCamera()
{
	return &camera_main;
}

void Antimony::detachCamera()
{
	if (camera_main.object)
		camera_main.displacement = camera_main.object->getFloat3Pos() + camera_main.displacement;
	camera_main.object = nullptr;
	camera_main.enableZoom(ON);
	camera_main.enableDolly(ON);
	camera_main.noclip(ON);

	camera_main.zoom = 1;
	camera_main.dolly = 1;
}
void Antimony::attachCamera(btObject *object, float3 displ, bool zoom, bool dolly)
{
	camera_main.object = object;
	camera_main.displacement = displ;
	camera_main.enableZoom(zoom);
	camera_main.enableDolly(dolly);
	camera_main.noclip(OFF);

	camera_main.zoom = 1;
	camera_main.dolly = 1;
}
void Antimony::attachCamera(float3 pos, bool zoom, bool dolly)
{
	camera_main.object = nullptr;
	camera_main.displacement = pos;
	camera_main.enableZoom(zoom);
	camera_main.enableDolly(dolly);
	camera_main.noclip(OFF);

	camera_main.zoom = 1;
	camera_main.dolly = 1;
}

unsigned char Antimony::subSystem;

void Antimony::setSubSystem(unsigned char subs)
{
	subSystem = subs;
}
unsigned char Antimony::getSubSystem()
{
	return subSystem;
}
bool Antimony::ifSubSystem(unsigned char subs)
{
	if (subSystem == subs)
		return true;
	else
		return false;
}


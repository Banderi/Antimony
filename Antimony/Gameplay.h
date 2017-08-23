#pragma once

#include "Player.h"
#include "Camera.h"

///

namespace Antimony
{
	extern Player player;
	extern Camera camera_main;

	Player* getPlayer();
	Camera* getCamera();
	void detachCamera();
	void attachCamera(btObject *object, float3 displ, bool zoom = OFF, bool dolly = OFF);
	void attachCamera(float3 pos, bool zoom = OFF, bool dolly = OFF);

	extern unsigned char subSystem;

	void setSubSystem(unsigned char subs);
	unsigned char getSubSystem();
	bool ifSubSystem(unsigned char subs);
}
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
	void attachCamera(btObject *object, float3 displ, float zoom = 1, float dolly = 1, bool en_zoom = OFF, bool en_dolly = OFF);
	void attachCamera(float3 pos, float zoom = 1, float dolly = 1, bool en_zoom = OFF, bool en_dolly = OFF);
}
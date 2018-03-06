#include "Gameplay.h"

///

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
void Antimony::attachCamera(btObject *object, float3 displ, float zoom, float dolly, bool en_zoom, bool en_dolly)
{
	camera_main.object = object;
	camera_main.displacement = displ;
	camera_main.zoom = zoom;
	camera_main.dolly = dolly;
	camera_main.enableZoom(en_zoom);
	camera_main.enableDolly(en_dolly);
	camera_main.noclip(OFF);
}
void Antimony::attachCamera(float3 pos, float zoom, float dolly, bool en_zoom, bool en_dolly)
{
	camera_main.object = nullptr;
	camera_main.displacement = pos;
	camera_main.zoom = zoom;
	camera_main.dolly = dolly;
	camera_main.enableZoom(en_zoom);
	camera_main.enableDolly(en_dolly);
	camera_main.noclip(OFF);
}

namespace Antimony
{
	Player player;
	Camera camera_main;
}
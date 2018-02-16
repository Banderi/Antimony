#pragma once

#include "Player.h"
#include "Camera.h"

///

#define SUBSYS_NONE				0x00

#define SUBSYS_FPS_TPS			0x10		// 3D First/third Person Shooter
#define SUBSYS_RTS				0x11		// 3D Real-time strategy
#define SUBSYS_TILED			0x12		// 2D tiled
#define SUBSYS_SCROLLER			0x13		// 2D/3D side-scroller

#define SUBSYS_VISUAL			0x20		// Visual novel
#define SUBSYS_FISHEYE			0x21		// 360° Fisheye
#define SUBSYS_ADVENTURE		0x22		// Point-and-click adventure

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
#pragma once

#include "Player.h"
#include "Camera.h"

///

//		*** Subsystems ***
//		Subsystems are collections of default parameters and settings used to set up the engine according to a desired type of game;

#define SUBSYS_NONE				0x00		// no subsystem, null/default value.			Physics			Camera			Cursor			Keyboard

#define SUBSYS_CUSTOM			0x01		// custom ruleset, manually set up				-				-				-				-

#define SUBSYS_FPS_TPS			0x10		// 3D First/third Person Shooter				Yes				First Pers.		Exclusive		Movements
#define SUBSYS_SCROLLER			0x12		// 2D/3D side-scroller							Yes				Third Pers.		Exclusive		Movements
#define SUBSYS_RTS				0x11		// 3D Real-time strategy						Yes				Panoramic		Manual			Functions
#define SUBSYS_TILED			0x13		// 2D tiled										No				Pan				Manual			Functions
#define SUBSYS_VISUAL			0x14		// Visual novel									No				N/A				Manual			Functions
#define SUBSYS_FISHEYE			0x15		// 360° Fisheye									No				Orbit			Manual*			Functions
#define SUBSYS_ADVENTURE		0x16		// Point-and-click adventure					No				Fixed			Manual			Movements

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
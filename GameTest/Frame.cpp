#include <string>
#include <fstream>
#include <vector>

//#include "Warnings.h"
#include "..\Antimony\Antimony.h"
#include "..\Antimony\Gameflow.h"
#include "..\Antimony\Player.h"
//#include "..\Antimony\CpuUsage.h"
//#include "..\Antimony\Timer.h"
//#include "Bullet.h"
//#include "FBX.h"
#include "Frame.h"

#pragma comment (lib, "Shlwapi.lib")

///

using namespace std;

///

wchar_t global_str64[64];
int frame_count;

///

void Frame()
{
	RenderWorld();														// render world
	RenderEntities();													// render entities
	RenderHUD();														// render HUD
}

void RenderWorld()
{
	devcon->IASetVertexBuffers(0, 1, &vertexbuffer, &vertex_stride, &vertex_offset);
	setDepthBufferState(ON);
	setShader(SHADERS_DEBUG);

	// ground wireframe
	Draw3DLineThin(WORLD_SCALE * float3(-2, 0, -2), WORLD_SCALE * float3(-2, 0, 2), COLOR_BLACK, COLOR_BLACK, &mat_identity);
	Draw3DLineThin(WORLD_SCALE * float3(0, 0, -2), WORLD_SCALE * float3(0, 0, 2), COLOR_BLACK, COLOR_BLACK, &mat_identity);
	Draw3DLineThin(WORLD_SCALE * float3(2, 0, -2), WORLD_SCALE * float3(2, 0, 2), COLOR_BLACK, COLOR_BLACK, &mat_identity);
	Draw3DLineThin(WORLD_SCALE * float3(-2, 0, -2), WORLD_SCALE * float3(2, 0, -2), COLOR_BLACK, COLOR_BLACK, &mat_identity);
	Draw3DLineThin(WORLD_SCALE * float3(-2, 0, 0), WORLD_SCALE * float3(2, 0, 0), COLOR_BLACK, COLOR_BLACK, &mat_identity);
	Draw3DLineThin(WORLD_SCALE * float3(-2, 0, 2), WORLD_SCALE * float3(2, 0, 2), COLOR_BLACK, COLOR_BLACK, &mat_identity);
	Draw3DLineThin(WORLD_SCALE * float3(0, 0, 0), WORLD_SCALE * float3(0, 0.5, 0), COLOR_BLACK, COLOR_BLACK, &mat_identity);
}
void RenderEntities()
{
	devcon->IASetVertexBuffers(0, 1, &vertexbuffer, &vertex_stride, &vertex_offset);
	setDepthBufferState(ON);
	setShader(SHADERS_DEBUG);

	// entities
	for (unsigned int i = 0; i < antimony.getEntities()->size(); i++)
	{
		if (antimony.getEntities()->at(i)->getKind() == BTOBJECT_PLAYER)
			continue;
		mat_world = antimony.getEntities()->at(i)->getMatTransform();
		color c;
		int state = antimony.getEntities()->at(i)->getRigidBody()->getActivationState();


		if (antimony.getEntities()->at(i)->getKind() == BTOBJECT_STATICWORLD)
			c = COLOR_WHITE;
		else if (antimony.getEntities()->at(i)->getKind() == BTOBJECT_KINEMATICWORLD)
			c = color(2, 1, 1, 1);
		else
		{
			switch (state)
				{
				case ACTIVE_TAG:
				{
					c = color(0.9, 1.1, 2, 1);
					break;
				}
				case ISLAND_SLEEPING:
				{
					c = COLOR_WHITE;
					break;
				}
				case WANTS_DEACTIVATION:
				{
					c = COLOR_RED;
					break;
				}
				default:
				{
					c = COLOR_WHITE;
					break;
				}
			}
		}

		switch (antimony.getEntities()->at(i)->getPrimitive())
		{
			case BTSOLID_BOX:
			{
				Draw3DBox(antimony.getEntities()->at(i)->getPrimitiveSize(), c);
				break;
			}
			case BTSOLID_CYLINDER:
			{
				Draw3DBox(antimony.getEntities()->at(i)->getPrimitiveSize(), c);
				break;
			}
			default:
			{
				Draw3DBox(antimony.getEntities()->at(i)->getPrimitiveSize(), c);
				break;
			}
		}

		mat_world = mat_identity;
	}

	// player
	mat_world = antimony.getPlayer()->getColl()->getMatTransform();
	if (antimony.getPlayer()->getJumpState() != JUMPSTATE_ONGROUND)
		Draw3DBox(WORLD_SCALE * 0.15, WORLD_SCALE * 0.3, WORLD_SCALE * 0.15, color(1, 1.1, 1.5, 1));
	else
		Draw3DBox(WORLD_SCALE * 0.15, WORLD_SCALE * 0.3, WORLD_SCALE * 0.15, COLOR_GREEN);
}
void RenderHUD()
{
	// TODO: Implement HUD

	static double pause_falloff = 0;
	pause_falloff += 2 * antimony.getDelta();
	if (pause_falloff > DX_PI)
		pause_falloff = 0;

	devcon->IASetVertexBuffers(0, 1, &vertexbuffer, &vertex_stride, &vertex_offset);
	setDepthBufferState(OFF);
	setShader(SHADERS_PLAIN);

	if (antimony.ifGameState(GAMESTATE_INGAME))									// In-game (non-paused, non-menu etc.)
	{
		//
	}
	else if (antimony.ifGameState(GAMESTATE_PAUSED))							// Game is paused
	{
		antimony.fw1Arial->DrawString(devcon, L"GAME PAUSED", 30, antimony.display.right, 200, RGBA2DWORD(0xff, 0xff, 0xff, 0xff * sinf(pause_falloff)), FW1_CENTER);
	}
	else if (antimony.ifGameState(GAMESTATE_PAUSEMENU))							// Pause Menu (in-antimony.game inventory/pause menu)
	{
		Draw2DRectangle(antimony.display.width, antimony.display.height, antimony.display.left, antimony.display.bottom, color(0, 0, 0, 0.25));
	}
}
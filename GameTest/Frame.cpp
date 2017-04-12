#include <string>
#include <fstream>
#include <vector>

//#include "Warnings.h"
#include "..\Antimony\Antimony.h"
#include "..\Antimony\Gameflow.h"
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
	//
}
void RenderEntities()
{
	//
}
void RenderHUD()
{
	// TODO: Implement HUD

	devcon->IASetVertexBuffers(0, 1, &vertexbuffer, &vertex_stride, &vertex_offset);
	setDepthBufferState(OFF);
	setShader(SHADERS_PLAIN);

	if (antimony.ifGameState(GAMESTATE_INGAME))									// INGAME (non-paused, non-menu etc.)
	{
		//
	}
	else if (antimony.ifGameState(GAMESTATE_PAUSE))								// PAUSED (in-antimony.game inventory/pause menu)
	{
		Draw2DRectangle(antimony.display.width, antimony.display.height, antimony.display.left, antimony.display.bottom, color(0, 0, 0, 0.25));
	}
}
#include <string>
#include <fstream>
#include <vector>

//#include "Warnings.h"
#include "..\Antimony\Antimony.h"
#include "..\Antimony\Gameflow.h"
#include "..\Antimony\Gameplay.h"
#include "..\Antimony\Param.h"
#include "..\Antimony\FontRenderer.h"
#include "..\Antimony\Step.h"
//#include "..\Antimony\CpuUsage.h"
#include "..\Antimony\Timer.h"
//#include "Bullet.h"
//#include "FBX.h"
#include "Frame.h"

///

using namespace std;

///

wchar_t global_str64[64];
int frame_count;

///

void Frame()
{
	RenderEntities();													// render entities
	RenderWorld();														// render world
	RenderHUD();														// render HUD
}

void Update_FPS()
{
	static double h = 0;
	double h_sp = 0.5f;
	if (Antimony::ifGameState(GAMESTATE_INGAME))
		h += h_sp * MATH_PI * Antimony::delta;
	if (h >= 2 * MATH_PI)
		h = 0;

	if (0)
	{
		unsigned int f = 0;
		f = abs(50 * cosf(h) - 40);
		Sleep(f);
	}

	Antimony::physEntities.at(3)->setMatTransform(&(MTranslation(0, 0.5, 0) * MRotY(h)));
	Antimony::physEntities.at(4)->setMatTransform(&(MTranslation(3, 1, sinf(h))));
	Antimony::physEntities.at(5)->setMatTransform(&(MTranslation(3, (1 - 0.5 * sinf(h)), 2)));
}
void Update_TPS()
{
	static double h = 0;
	double h_sp = 0.5f;
	if (Antimony::ifGameState(GAMESTATE_INGAME))
		h += h_sp * MATH_PI * Antimony::delta;
	if (h >= 2 * MATH_PI)
		h = 0;

	if (0)
	{
		unsigned int f = 0;
		f = abs(50 * cosf(h) - 40);
		Sleep(f);
	}

	Antimony::physEntities.at(3)->setMatTransform(&(MTranslation(0, 0.5, 0) * MRotY(h)));
	Antimony::physEntities.at(4)->setMatTransform(&(MTranslation(3, 1, sinf(h))));
	//physEntities.at(4)->updateKinematic(delta);
	Antimony::physEntities.at(5)->setMatTransform(&(MTranslation(3, (1 - 0.5 * sinf(h)), 2)));
	//physEntities.at(5)->updateKinematic(delta);
	//physEntities.at(5)->getRigidBody()->setLinearVelocity(btVector3(0, (sinf(h)), 0));
	//physEntities.at(5)->getRigidBody()->setLinearFactor(btVector3(0, (sinf(h)), 0));
	//physEntities.at(5)->getRigidBody()->setLinearVelocity(bt_origin);
}
void Update_Scroller()
{

}
void Update_RTS()
{

}
void Update_Tiled()
{

}
void Update_Visual()
{

}
void Update_Fisheye()
{

}
void Update_Adventure()
{

}

void RenderWorld()
{
	//devcon->IASetVertexBuffers(0, 1, &vertexbuffer, (UINT*)sizeof(VERTEX_BASIC), (UINT*)(0));
	Antimony::setDepthBufferState(ON);
	Antimony::setShader(SHADERS_DEBUG);

	// ground wireframe
	Draw3DLineThin( float3(-2, 0, -2), float3(-2, 0, 2), COLOR_BLACK, COLOR_BLACK, &mat_identity);
	Draw3DLineThin( float3(0, 0, -2), float3(0, 0, 2), COLOR_BLACK, COLOR_BLACK, &mat_identity);
	Draw3DLineThin( float3(2, 0, -2), float3(2, 0, 2), COLOR_BLACK, COLOR_BLACK, &mat_identity);
	Draw3DLineThin( float3(-2, 0, -2), float3(2, 0, -2), COLOR_BLACK, COLOR_BLACK, &mat_identity);
	Draw3DLineThin( float3(-2, 0, 0), float3(2, 0, 0), COLOR_BLACK, COLOR_BLACK, &mat_identity);
	Draw3DLineThin( float3(-2, 0, 2), float3(2, 0, 2), COLOR_BLACK, COLOR_BLACK, &mat_identity);
	Antimony::setDepthBufferState(ON);
	Draw3DLineThin( float3(0, 0, 0), float3(0, 0.5, 0), COLOR_BLACK, COLOR_BLACK, &mat_identity);

	auto _p = WorldToScreen(float3(1, 1, 1), &(mat_view * mat_proj), float2(Antimony::display.width, Antimony::display.height));
	if (_p.z > 0)
		Draw2DDot(float2(_p.x, _p.y), 4, COLOR_WHITE);

	Antimony::timer.catchTime(TIMER_RENDER_WORLD);
}
void RenderEntities()
{
	//devcon->IASetVertexBuffers(0, 1, &vertexbuffer, (UINT*)sizeof(VERTEX_BASIC), (UINT*)(0));
	Antimony::setDepthBufferState(ON);
	Antimony::setShader(SHADERS_PLAIN3D);

	// entities
	for (unsigned int i = 0; i < Antimony::physEntities.size(); i++)
	{
		if (Antimony::physEntities.at(i)->getKind() == BTOBJECT_PLAYER)
			continue;
		mat_world = Antimony::physEntities.at(i)->getMatTransform();
		color c;
		int state = Antimony::physEntities.at(i)->getRigidBody()->getActivationState();


		if (Antimony::physEntities.at(i)->getKind() == BTOBJECT_STATICWORLD)
			c = color(0.3, 0.3, 0.3, 1);
		else if (Antimony::physEntities.at(i)->getKind() == BTOBJECT_KINEMATICWORLD)
			c = color(0.6, 0.0, 0.0, 1);
		else
		{
			switch (state)
				{
				case ACTIVE_TAG:
				{
					c = color(0.25, 0.4, 0.6, 1);
					break;
				}
				case ISLAND_SLEEPING:
				{
					c = color(0.4, 0.4, 0.4, 1);
					break;
				}
				case WANTS_DEACTIVATION:
				{
					c = color(0.3, 0.1, 0.1, 1);
					break;
				}
				default:
				{
					c = color(0.6, 0.6, 0.6, 1);
					break;
				}
			}
		}

		switch (Antimony::physEntities.at(i)->getPrimitive())
		{
			case BTSOLID_BOX:
			{
				Draw3DBox(Antimony::physEntities.at(i)->getPrimitiveSize(), c);
				break;
			}
			case BTSOLID_CYLINDER:
			{
				Draw3DBox(Antimony::physEntities.at(i)->getPrimitiveSize(), c);
				break;
			}
			case BTSOLID_INFPLANE:
			{
				break;
			}
			default:
			{
				Draw3DBox(Antimony::physEntities.at(i)->getPrimitiveSize(), c);
				break;
			}
		}

		mat_world = mat_identity;
	}

	// player
	mat_world = Antimony::getPlayer()->getTransform();
	mat_temp = XMMatrixScaling(0.01, 0.01, 0.01);

	//Antimony::setShader(SHADERS_PLAIN);

	Antimony::getPlayer()->getAsset()->draw(&(mat_world), Antimony::game.dbg_info);
	/*if (Antimony::game.dbg_wireframe)
	{
		mat_world = Antimony::getPlayer()->getColl()->getMatTransform();
		if (Antimony::getPlayer()->getJumpState() != JUMPSTATE_ONGROUND)
			Draw3DBox( 0.15, 0.3, 0.15, color(1, 1.1, 1.5, 0.02));
		else
			Draw3DBox( 0.15, 0.3, 0.15, color(0.9, 1.5, 1.5, 0.02));
	}*/

	mat_world = MTranslation(0, -0.01, 0);
	Draw3DRectangle(50, 50, color(0.3, 0.3, 0.3, 1));

	Antimony::timer.catchTime(TIMER_RENDER_ENTITIES);
}
void RenderHUD()
{
	// TODO: Implement HUD

	static double pause_falloff = 0;
	pause_falloff += 2 * Antimony::getDelta();
	if (pause_falloff > MATH_PI)
		pause_falloff = 0;

	//devcon->IASetVertexBuffers(0, 1, &vertexbuffer, (UINT*)sizeof(VERTEX_BASIC), (UINT*)(0));
	Antimony::setDepthBufferState(OFF);
	Antimony::setShader(SHADERS_PLAIN);

	if (Antimony::ifGameState(GAMESTATE_INGAME))									// In-game (non-paused, non-menu etc.)
	{
		pause_falloff = 0;
	}
	else if (Antimony::ifGameState(GAMESTATE_PAUSED))								// Game is paused
	{
		Antimony::setDepthBufferState(ON);
		mat_world = mat_identity;
		float3 pos = Antimony::getCamera()->displacement;
		float3 px = pos + float3(0.1, 0, 0);
		float3 py = pos + float3(0, 0.1, 0);
		float3 pz = pos + float3(0, 0, 0.1);
		Draw3DLineThin(pos, px, COLOR_RED, COLOR_RED);
		Draw3DLineThin(pos, py, COLOR_GREEN, COLOR_GREEN);
		Draw3DLineThin(pos, pz, COLOR_BLUE, COLOR_BLUE);
		Antimony::setDepthBufferState(OFF);
		float tr = 0.3;
		Draw3DLineThin(pos, px, COLOR_RED * tr, COLOR_RED * tr);
		Draw3DLineThin(pos, py, COLOR_GREEN * tr, COLOR_GREEN * tr);
		Draw3DLineThin(pos, pz, COLOR_BLUE * tr, COLOR_BLUE * tr);

		Antimony::Arial.render(L"GAME PAUSED", 30, Antimony::display.right, 200, RGBA2DWORD(0xff, 0xff, 0xff, 0xff * sinf(pause_falloff)), FW1_CENTER);
	}
	else if (Antimony::ifGameState(GAMESTATE_PAUSEMENU))							// Pause Menu (in-Antimony::game inventory/pause menu)
	{
		Draw2DRectangle(Antimony::display.width, Antimony::display.height, Antimony::display.left, Antimony::display.bottom, color(0, 0, 0, 0.25));
	}

	Antimony::timer.catchTime(TIMER_RENDER_HUD);
}
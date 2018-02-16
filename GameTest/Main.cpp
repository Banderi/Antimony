#include "resource.h"
#include "Main.h"
#include "Frame.h"

#include "..\Antimony\Input.h"
#include "..\Antimony\Step.h"
#include "..\Antimony\Console.h"
#include "..\Antimony\Bullet.h"
#include "..\Antimony\Gameplay.h"
#include "..\Antimony\Gameflow.h"

#pragma comment (lib, "Shlwapi.lib")

///

using namespace std;

///

int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{

	Antimony::setSubSystem(SUBSYS_FPS_TPS);

	if (Antimony::startUp(hInstance, nCmdShow) == 0)
		return 0;

	///

	unsigned int ENGINETEST = SUBSYS_FPS_TPS;

	switch (ENGINETEST)
	{
	case SUBSYS_FPS_TPS:
		Init_TPS();
		break;
	case SUBSYS_RTS:
		Init_RTS();
		break;
	case SUBSYS_TILED:
		Init_Tiled();
		break;
	case SUBSYS_SCROLLER:
		Init_Scroller();
		break;
	case SUBSYS_VISUAL:
		Init_Visual();
		break;
	case SUBSYS_FISHEYE:
		Init_Fisheye();
		break;
	case SUBSYS_ADVENTURE:
		Init_Adventure();
		break;
	}

	///

	MSG msg;
	bool run = 1;

	while (run)
	{
		Antimony::wm_message = false;
		Antimony::wm_input = false;
		Antimony::wm_keydown = false;

		while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
		{
			Antimony::wm_message = true;
			switch (msg.message)
			{
				case WM_QUIT:
				{
					run = 0;
					break;
				}
				case WM_INPUT:
				{
					Antimony::wm_input = true;
					Antimony::handleInput(msg);
					TranslateMessage(&msg);
					DispatchMessageW(&msg);
					break;
				}
				case WM_KEYDOWN:
				{
					Antimony::wm_keydown = true;
					Antimony::handleInput(msg);
					TranslateMessage(&msg);
					DispatchMessageW(&msg);
					break;
				}
				default:
				{
					TranslateMessage(&msg);
					DispatchMessageW(&msg);
					break;
				}
			}
		}

		Antimony::step();
		Frame();
		Antimony::endStep();
	}

	Antimony::cleanUp();

	return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
			break;
		}
		case WM_KEYDOWN:
		{
			if (Antimony::devConsole.isClosed())
			{
				switch (wParam)
				{
					case VK_F3:
					{
						Antimony::game.dbg_info = !Antimony::game.dbg_info;
						break;
					}
					case VK_F4:
					{
						Antimony::game.dbg_infochart++;
						if (Antimony::game.dbg_infochart > DBGCHART_PAGES_COUNT - 1)
						{
							Antimony::game.dbg_infochart = 0;
						}
						break;
					}
					case VK_F8:
					{
						Antimony::game.dbg_wireframe = !Antimony::game.dbg_wireframe;
						break;
					}
					case VK_OEM_COMMA:
					{
						Antimony::game.dbg_entityfollow--;
						if (Antimony::game.dbg_entityfollow < 0)
							Antimony::game.dbg_entityfollow = Antimony::physEntities.size() - 1;
						break;
					}
					case VK_OEM_PERIOD:
					{
						Antimony::game.dbg_entityfollow++;
						if (Antimony::game.dbg_entityfollow > Antimony::physEntities.size() - 1)
							Antimony::game.dbg_entityfollow = 0;
						break;
					}
					case 0x52:
					{
						// TODO: fix platform momentum
						Antimony::resetPhysics();
						break;
					}
					case 0x46:
					{
						if (Antimony::devConsole.isClosed())
						{
							if (Antimony::camera_main.object == nullptr)
							{
								Antimony::setGameState(GAMESTATE_INGAME);
								Antimony::attachCamera(Antimony::getPlayer()->getColl(), float3(0, 0.26, 0));
							}
							else
							{
								Antimony::setGameState(GAMESTATE_PAUSED);
								Antimony::detachCamera();
							}
						}
						break;
					}
				}
			}
		}
	}
	return DefWindowProcW(hWnd, message, wParam, lParam);
}

void Init_TPS()
{
	// test walls
	Antimony::addPhysEntity(BTOBJECT_STATICWORLD, BTSOLID_BOX, 0.0f, float3(0.3, 2, 5), float4(0, 0, 0, 1), float3(-3, 1, -1), float3(0, 0, 0), Antimony::btWorld);

	// moving platforms
	Antimony::addPhysEntity(BTOBJECT_KINEMATICWORLD, BTSOLID_BOX, 0.0f, float3(0.45, 0.15, 0.45), float3(0, 0.5, 0), float3(0, 0, 0), Antimony::btWorld);
	Antimony::addPhysEntity(BTOBJECT_KINEMATICWORLD, BTSOLID_BOX, 0.0f, float3(0.45, 0.15, 0.45), float3(3, 1, 0), float3(0, 0, 0), Antimony::btWorld);
	Antimony::addPhysEntity(BTOBJECT_KINEMATICWORLD, BTSOLID_BOX, 0.0f, float3(0.45, 0.15, 0.45), float3(3, 2, 2), float3(0, 0, 0), Antimony::btWorld);

	// test cubes
	Antimony::addPhysEntity(BTOBJECT_DYNAMIC, BTSOLID_BOX, 10.0f, float3(0.3, 0.3, 0.3), float3(0, 6, 0), Antimony::btWorld);
	Antimony::addPhysEntity(BTOBJECT_DYNAMIC, BTSOLID_BOX, 10.0f, float3(0.3, 0.3, 0.3), float3(0, 40, 0), Antimony::btWorld);
	Antimony::addPhysEntity(BTOBJECT_DYNAMIC, BTSOLID_BOX, 10.0f, float3(0.3, 0.3, 0.3), float3(0, 80, 0), Antimony::btWorld);
	Antimony::addPhysEntity(BTOBJECT_DYNAMIC, BTSOLID_BOX, 10.0f, float3(0.3, 0.3, 0.3), float3(3, 4, 2), Antimony::btWorld);

	Antimony::getPlayer()->asset.loadFBX(L"chibi_wolf_anim.fbx", float3(0.2));
	Antimony::attachCamera(Antimony::getPlayer()->getColl(), float3(0, 0.26, 0), false, true);
}
void Init_RTS()
{

}
void Init_Tiled()
{

}
void Init_Scroller()
{

}
void Init_Visual()
{

}
void Init_Fisheye()
{

}
void Init_Adventure()
{

}
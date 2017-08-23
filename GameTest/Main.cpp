#include "resource.h"
#include "Main.h"
#include "Frame.h"

#include "..\Antimony\Input.h"
#include "..\Antimony\Step.h"
#include "..\Antimony\Console.h"
#include "..\Antimony\Bullet.h"
#include "..\Antimony\Gameplay.h"
#include "..\Antimony\Gameflow.h"

//#if _WIN32 || _WIN64
//#if _WIN64
//#define ENVIRONMENT64
//#else
//#define ENVIRONMENT32
//#endif
//#endif
//
//#ifdef ENVIRONMENT32
//#ifdef _DEBUG
//#pragma comment (lib, "..\\ext\\FBXSDK\\lib\\x86\\libfbxsdk-static_d.lib")
//#else
//#pragma comment (lib, "..\\ext\\FBXSDK\\lib\\x86\\libfbxsdk.lib")
//#endif
//#elif defined(ENVIRONMENT64)
//#ifdef _DEBUG
//#pragma comment (lib, "..\\ext\\FBXSDK\\lib\\x64\\libfbxsdk-static_d.lib")
//#else
//#pragma comment (lib, "..\\ext\\FBXSDK\\lib\\x64\\libfbxsdk-static.lib")
//#endif
//#endif

///

using namespace std;

///

int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{

	if (Antimony::startUp(hInstance, nCmdShow) == 0)
		return 0;

	Temp_StartingFiles(); //TODO: implement file loading (LUA?)

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
			if (!Antimony::devConsole.isOpen())
			{
				switch (wParam)
				{
					case VK_F6:
					{
						Antimony::game.dbg_info = !Antimony::game.dbg_info;
						break;
					}
					case VK_F7:
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
							Antimony::game.dbg_entityfollow = Antimony::getEntities()->size() - 1;
						break;
					}
					case VK_OEM_PERIOD:
					{
						Antimony::game.dbg_entityfollow++;
						if (Antimony::game.dbg_entityfollow > Antimony::getEntities()->size() - 1)
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
						if (!Antimony::devConsole.isOpen())
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

void Temp_StartingFiles()
{
	btDefaultMotionState *ms;
	btObject *phys_obj;

	// test walls
	ms = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), WORLD_SCALE * btVector3(-3, 1, -1)));
	phys_obj = new btObject(BTOBJECT_STATICWORLD, BTSOLID_BOX, 0.0f, float3(0.3, 2, 5), ms, &btVector3(0, 0, 0), Antimony::getBtWorld());
	Antimony::addPhysEntity(phys_obj);

	// moving platforms
	ms = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), WORLD_SCALE * btVector3(0, 0.5, 0)));
	phys_obj = new btObject(BTOBJECT_KINEMATICWORLD, BTSOLID_BOX, 0.0f, float3(0.45, 0.15, 0.45), ms, &btVector3(0, 0, 0), Antimony::getBtWorld());
	Antimony::addPhysEntity(phys_obj);

	ms = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), WORLD_SCALE * btVector3(3, 1, 0)));
	phys_obj = new btObject(BTOBJECT_KINEMATICWORLD, BTSOLID_BOX, 0.0f, float3(0.45, 0.15, 0.45), ms, &btVector3(0, 0, 0), Antimony::getBtWorld());
	Antimony::addPhysEntity(phys_obj);

	ms = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), WORLD_SCALE * btVector3(3, 2, 2)));
	phys_obj = new btObject(BTOBJECT_KINEMATICWORLD, BTSOLID_BOX, 0.0f, float3(0.45, 0.15, 0.45), ms, &btVector3(0, 0, 0), Antimony::getBtWorld());
	Antimony::addPhysEntity(phys_obj);

	// test cubes
	ms = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), WORLD_SCALE * btVector3(0, 6, 0)));
	phys_obj = new btObject(BTOBJECT_DYNAMIC, BTSOLID_BOX, 10.0f, float3(0.3, 0.3, 0.3), ms, Antimony::getBtWorld());
	Antimony::addPhysEntity(phys_obj);

	ms = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), WORLD_SCALE * btVector3(0, 40, 0)));
	phys_obj = new btObject(BTOBJECT_DYNAMIC, BTSOLID_BOX, 10.0f, float3(0.3, 0.3, 0.3), ms, Antimony::getBtWorld());
	Antimony::addPhysEntity(phys_obj);

	ms = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), WORLD_SCALE * btVector3(0, 80, 0)));
	phys_obj = new btObject(BTOBJECT_DYNAMIC, BTSOLID_BOX, 10.0f, float3(0.3, 0.3, 0.3), ms, Antimony::getBtWorld());
	Antimony::addPhysEntity(phys_obj);

	ms = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), WORLD_SCALE * btVector3(3, 4, 2)));
	phys_obj = new btObject(BTOBJECT_DYNAMIC, BTSOLID_BOX, 10.0f, float3(0.3, 0.3, 0.3), ms, Antimony::getBtWorld());
	Antimony::addPhysEntity(phys_obj);

	Antimony::getPlayer()->asset.loadFBX(L"chibi_wolf_anim.fbx", float3(0.2));
	Antimony::attachCamera(Antimony::getPlayer()->getColl(), float3(0, 0.26, 0), false, true);
}
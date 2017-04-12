#include "resource.h"
#include "Main.h"
#include "Frame.h"

///

using namespace std;

///

int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{

	if (antimony.startUp(hInstance, nCmdShow) == 0)
		return 0;

	Temp_StartingFiles(); //TODO: implement file loading (LUA?)

	MSG msg;
	bool run = 1;

	while (run)
	{
		while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
		{
			switch (msg.message)
			{
				case WM_QUIT:
				{
					run = 0;
					break;
				}
				case WM_INPUT:
				{
					antimony.handleInput(msg);
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

		antimony.step();
		Frame();
		antimony.endStep();
	}

	antimony.cleanUp();

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
		switch (wParam)
		{
		case VK_ESCAPE:
		{
			//PostQuitMessage(0);
			break;
		}
		case VK_F7:
		{
			antimony.game.dbg_wireframe = !antimony.game.dbg_wireframe;
			break;
		}
		case VK_OEM_COMMA:
		{
			antimony.game.dbg_entityfollow--;
			if (antimony.game.dbg_entityfollow < 0)
				antimony.game.dbg_entityfollow = antimony.getEntityCount() - 1;
			break;
		}
		case VK_OEM_PERIOD:
		{
			antimony.game.dbg_entityfollow++;
			if (antimony.game.dbg_entityfollow > antimony.getEntityCount() - 1)
				antimony.game.dbg_entityfollow = 0;
			break;
		}
		case 0x52:
		{
			// TODO: fix platform momentum
			antimony.resetPhysics();
			break;
		}
		}
	}
	}
	return DefWindowProcW(hWnd, message, wParam, lParam);
}

void Temp_StartingFiles()
{
	btCollisionShape *cs;
	btDefaultMotionState *ms;
	btObject *phys_obj;

	// test walls
	cs = new btBoxShape(WORLD_SCALE * btVector3(0.3, 2, 5));
	ms = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), WORLD_SCALE * btVector3(-3, 1, -1)));
	phys_obj = new btObject(BTOBJECT_STATICWORLD, 0.0f, cs, ms, &btVector3(0, 0, 0), antimony.getBtWorld());

	// moving platforms
	cs = new btBoxShape(WORLD_SCALE * btVector3(0.45, 0.15, 0.45));
	ms = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), WORLD_SCALE * btVector3(0, 0.5, 0)));
	phys_obj = new btObject(BTOBJECT_KINEMATICWORLD, 0.0f, cs, ms, &btVector3(0, 0, 0), antimony.getBtWorld());

	cs = new btBoxShape(WORLD_SCALE * btVector3(0.45, 0.15, 0.45));
	ms = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), WORLD_SCALE * btVector3(3, 1, 0)));
	phys_obj = new btObject(BTOBJECT_KINEMATICWORLD, 0.0f, cs, ms, &btVector3(0, 0, 0), antimony.getBtWorld());

	cs = new btBoxShape(WORLD_SCALE * btVector3(0.45, 0.15, 0.45));
	ms = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), WORLD_SCALE * btVector3(3, 2, 2)));
	phys_obj = new btObject(BTOBJECT_KINEMATICWORLD, 0.0f, cs, ms, &btVector3(0, 0, 0), antimony.getBtWorld());

	// test cubes
	cs = new btBoxShape(WORLD_SCALE * btVector3(0.3, 0.3, 0.3));
	ms = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), WORLD_SCALE * btVector3(0, 6, 0)));
	phys_obj = new btObject(BTOBJECT_DYNAMIC, 10.0f, cs, ms, antimony.getBtWorld());

	ms = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), WORLD_SCALE * btVector3(0, 40, 0)));
	phys_obj = new btObject(BTOBJECT_DYNAMIC, 10.0f, cs, ms, antimony.getBtWorld());

	ms = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), WORLD_SCALE * btVector3(0, 80, 0)));
	phys_obj = new btObject(BTOBJECT_DYNAMIC, 10.0f, cs, ms, antimony.getBtWorld());

	ms = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), WORLD_SCALE * btVector3(3, 4, 2)));
	phys_obj = new btObject(BTOBJECT_DYNAMIC, 10.0f, cs, ms, antimony.getBtWorld());
}
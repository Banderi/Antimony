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
using namespace Antimony;

///

enum enginetest
{
	TEST_FPS,
	TEST_TPS,
	TEST_SCROLLER,
	TEST_RTS,
	TEST_TILED,
	TEST_VISUAL,
	TEST_FISHEYE,
	TEST_ADVENTURE
};

///

int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{

	if (startUp(hInstance, nCmdShow) == 0)
		return 0;

	///

	auto ENGINETEST = TEST_TPS;

	switch (ENGINETEST)
	{
	case TEST_FPS:
		Init_FPS();
		break;
	case TEST_TPS:
		Init_TPS();
		break;
	case TEST_SCROLLER:
		Init_Scroller();
		break;
	case TEST_RTS:
		Init_RTS();
		break;
	case TEST_TILED:
		Init_Tiled();
		break;
	case TEST_VISUAL:
		Init_Visual();
		break;
	case TEST_FISHEYE:
		Init_Fisheye();
		break;
	case TEST_ADVENTURE:
		Init_Adventure();
		break;
	}

	///

	MSG msg;
	bool run = 1;

	while (run)
	{
		messageQueue(&msg, &run);
		step();

		switch (ENGINETEST)
		{
		case TEST_FPS:
			Update_FPS();
			break;
		case TEST_TPS:
			Update_TPS();
			break;
		case TEST_SCROLLER:
			Update_Scroller();
			break;
		case TEST_RTS:
			Update_RTS();
			break;
		case TEST_TILED:
			Update_Tiled();
			break;
		case TEST_VISUAL:
			Update_Visual();
			break;
		case TEST_FISHEYE:
			Update_Fisheye();
			break;
		case TEST_ADVENTURE:
			Update_Adventure();
			break;
		}

		Frame();
		endStep();
	}

	cleanUp();

	return msg.wParam;
}

void Init_FPS()
{
	// test walls
	addPhysEntity(BTOBJECT_STATICWORLD, BTSOLID_BOX, 0.0f, float3(0.3, 2, 5), float4(0, 0, 0, 1), float3(-3, 1, -1), float3(0, 0, 0), btWorld);

	// moving platforms
	addPhysEntity(BTOBJECT_KINEMATICWORLD, BTSOLID_BOX, 0.0f, float3(0.45, 0.15, 0.45), float3(0, 0.5, 0), float3(0, 0, 0), btWorld);
	addPhysEntity(BTOBJECT_KINEMATICWORLD, BTSOLID_BOX, 0.0f, float3(0.45, 0.15, 0.45), float3(3, 1, 0), float3(0, 0, 0), btWorld);
	addPhysEntity(BTOBJECT_KINEMATICWORLD, BTSOLID_BOX, 0.0f, float3(0.45, 0.15, 0.45), float3(3, 2, 2), float3(0, 0, 0), btWorld);

	attachCamera(getPlayer()->getColl(), float3(0.0), 1, 0);
}
void Init_TPS()
{
	// test walls
	addPhysEntity(BTOBJECT_STATICWORLD, BTSOLID_BOX, 0.0f, float3(0.3, 2, 5), float4(0, 0, 0, 1), float3(-3, 1, -1), float3(0, 0, 0), btWorld);

	// moving platforms
	addPhysEntity(BTOBJECT_KINEMATICWORLD, BTSOLID_BOX, 0.0f, float3(0.45, 0.15, 0.45), float3(0, 0.5, 0), float3(0, 0, 0), btWorld);
	addPhysEntity(BTOBJECT_KINEMATICWORLD, BTSOLID_BOX, 0.0f, float3(0.45, 0.15, 0.45), float3(3, 1, 0), float3(0, 0, 0), btWorld);
	addPhysEntity(BTOBJECT_KINEMATICWORLD, BTSOLID_BOX, 0.0f, float3(0.45, 0.15, 0.45), float3(3, 2, 2), float3(0, 0, 0), btWorld);

	// test cubes
	addPhysEntity(BTOBJECT_DYNAMIC, BTSOLID_BOX, 10.0f, float3(0.3, 0.3, 0.3), float3(0, 6, 0), btWorld);
	addPhysEntity(BTOBJECT_DYNAMIC, BTSOLID_BOX, 10.0f, float3(0.3, 0.3, 0.3), float3(0, 40, 0), btWorld);
	addPhysEntity(BTOBJECT_DYNAMIC, BTSOLID_BOX, 10.0f, float3(0.3, 0.3, 0.3), float3(0, 80, 0), btWorld);
	addPhysEntity(BTOBJECT_DYNAMIC, BTSOLID_BOX, 10.0f, float3(0.3, 0.3, 0.3), float3(3, 4, 2), btWorld);

	getPlayer()->load3DAsset(L"chibi_wolf_anim.fbx", float3(0.2));
	attachCamera(getPlayer()->getColl(), float3(0, 0.26, 0), 1, 1, false, true);
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
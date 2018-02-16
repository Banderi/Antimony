#pragma once

#include <Windows.h>

//#include "fbxsdk.h"
//
//#include "Console.h"
////#include "Param.h"
////#include "Input.h"
////#include "Bullet.h"
////#include "Geometry.h"
//#include "FontRenderer.h"
//#include "CpuUsage.h"
//#include "Player.h"
//#include "Camera.h"
//#include "Spawner.h"

///

namespace Antimony
{
	int startUp(HINSTANCE hInstance, int nCmdShow);
	void readConfig();
	HRESULT enumHardware();
	HRESULT initDirectX11(HWND hWnd);
	HRESULT initControls();
	HRESULT initShaders();
	HRESULT initFonts();
	HRESULT initGraphics();
	HRESULT initPhysics();
	HRESULT initAssetLoaders();

	void createMainWindow(HINSTANCE hInstance);

	void cleanUp();
	void cleanD3D();
	void releaseFiles();
}
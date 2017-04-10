#pragma once

#include "..\Antimony\Antimony.h"
#pragma comment(lib, "antimony.lib")

///

void ReadConfig();
void Log();
HRESULT EnumHardware();
HRESULT InitD3D(HWND hWnd);
void CleanD3D();
HRESULT InitControls();
HRESULT LoadStartingFiles();
HRESULT InitShaders();
HRESULT InitFonts();
HRESULT InitGraphics();
HRESULT InitPhysics();
void ReleaseFiles();
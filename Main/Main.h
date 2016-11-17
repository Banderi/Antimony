#pragma once

#include <Windows.h>

//

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
void ReleaseFiles();
#ifndef MAIN_H
#define MAIN_H

#include <Windows.h>

void ReadConfig();
void Log();
HRESULT EnumHardware();
HRESULT InitD3D(HWND hWnd);
void CleanD3D();
HRESULT LoadStartingFiles();
HRESULT InitShaders();
HRESULT InitGraphics();
void ReleaseFiles();

template <typename T> void smartRelease(T com)
{
	if (com)
	{
		com->Release();
		com = nullptr;
	}
}

#endif
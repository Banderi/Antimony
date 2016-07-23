#ifndef HRES_H
#define HRES_H

#include <Windows.h>

bool Handle(HRESULT *hOut, DWORD facing, HRESULT hr);

#define HRH_MAIN_INITD3D 0
#define HRH_MAIN_REGHID 1

#define HRH_SWAPCHAIN_CREATE 10
#define HRH_SWAPCHAIN_SURFACEBUFFER 11
#define HRH_RENDERTARGET_CREATE 12
#define HRH_DEPTHSTENCIL_TEXTURE 13
#define HRH_DEPTHSTENCIL_STATE 14
#define HRH_DEPTHSTENCIL_VIEW 15
#define HRH_RASTERIZER_STATE 16

#define HRH_SHADER_COMPILE 20
#define HRH_SHADER_CREATE 21
#define HRH_SHADER_INPUTLAYOUT 22

#define HRH_GRAPHICS_VERTEXBUFFER 30
#define HRH_GRAPHICS_CONSTANTBUFFER 31
#define HRH_GRAPHICS_INDEXBUFFER 32

#endif
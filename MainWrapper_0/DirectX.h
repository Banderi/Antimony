#ifndef DIRECTX_H
#define DIRECTX_H

#include <d3d11_1.h>
#pragma comment (lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

#ifndef D3DX
//#define D3DX
#endif

#ifdef D3DX

#include <d3dx11.h>
#include <d3dx10.h>

#pragma comment (lib, "d3dx11.lib")
#pragma comment (lib, "d3dx10.lib")

#define float2 D3DXVECTOR2
#define float3 D3DXVECTOR3
#define float4 D3DXVECTOR4
#define mat D3DXMATRIX
#define color D3DXCOLOR

#else

#include <D3Dcompiler.h>

#include "SimpleMath.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

#pragma comment (lib, "D3DCompiler.lib")

#define DX_PI (3.14159265358979323846)

#define vec XMVECTOR

#define float2 Vector2
#define float3 Vector3
#define float4 Vector4
#define mat XMMATRIX
#define color Vector4

typedef float RGBA[4];

#endif

#endif
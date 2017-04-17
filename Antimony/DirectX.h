#pragma once

#include <d3d11_1.h>
#pragma comment (lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

#ifdef D3DX

#include <d3dx11.h>
#include <d3dx10.h>

#pragma comment (lib, "d3dx11.lib")
#pragma comment (lib, "d3dx10.lib")

typedef D3DXVECTOR2 float2
typedef D3DXVECTOR3 float3
typedef D3DXVECTOR4 float4
typedef D3DXMATRIX mat
typedef D3DXCOLOR color

#else

#include <D3Dcompiler.h>

#include "SimpleMath.h"

#pragma comment (lib, "D3DCompiler.lib")

///

using namespace DirectX;
using namespace DirectX::SimpleMath;

typedef float RGBA[4];
typedef Vector4 color;
typedef XMVECTOR vec;
typedef XMMATRIX mat;

typedef Vector2 float2;
typedef Vector3 float3;
typedef Vector4 float4;

#define MSet XMMatrixSet

#define MTranslation XMMatrixTranslation
#define MTranslVector XMMatrixTranslationFromVector
#define MScaling XMMatrixScaling
#define MScalVector XMMatrixScalingFromVector
#define MRotX XMMatrixRotationX
#define MRotY XMMatrixRotationY
#define MRotZ XMMatrixRotationZ
#define MRotRPY XMMatrixRotationRollPitchYaw
#define MRotRPYVector XMMatrixRotationRollPitchYawFromVector
#define MRotNormal XMMatrixRotationNormal
#define MRotAxis XMMatrixRotationAxis
#define MRotQuaternion XMMatrixRotationQuaternion

#define MTransformation XMMatrixTransformation
#define MTransf2D XMMatrixTransformation2D
#define MTransfAffine XMMatrixAffineTransformation
#define MTransfAffine2D XMMatrixAffineTransformation2D
#define MReflect XMMatrixReflect
#define MShadow XMMatrixShadow
#define MIdentity XMMatrixIdentity
#define MMultiply XMMatrixMultiply
#define MMultTranspose XMMatrixMultiplyTranspose
#define MTranspose XMMatrixTranspose
#define MInverse XMMatrixInverse
#define MDeterminant XMMatrixDeterminant
#define MDecompose XMMatrixDecompose

#define MIsNaN XMMatrixIsNaN
#define MIsInfinite XMMatrixIsInfinite
#define MIsIdentity XMMatrixIsIdentity

#define MLookAtLH XMMatrixLookAtLH
#define MLookAtRH XMMatrixLookAtRH
#define MLookToLH XMMatrixLookToLH
#define MLookToRH XMMatrixLookToRH
#define MPerspLH XMMatrixPerspectiveLH
#define MPerspRH XMMatrixPerspectiveRH
#define MPerspFovLH XMMatrixPerspectiveFovLH
#define MPerspFovRH XMMatrixPerspectiveFovRH
#define MPerspOffCenterLH XMMatrixPerspectiveOffCenterLH
#define MPerspOffCenterRH XMMatrixPerspectiveOffCenterRH
#define MOrthoLH XMMatrixOrthographicLH
#define MOrthoRH XMMatrixOrthographicRH
#define MOrthoOffCenterLH XMMatrixOrthographicOffCenterLH
#define MOrthoOffCenterRH XMMatrixOrthographicOffCenterRH

#define COLOR_WHITE		color(1,1,1,1)
#define COLOR_BLACK		color(0,0,0,1)
#define COLOR_RED		color(1,0,0,1)
#define COLOR_GREEN		color(0,1,0,1)
#define COLOR_BLUE		color(0,0,1,1)

#define DX_PI (3.14159265358979323846)

///

extern int frame_count;

#endif
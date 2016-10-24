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

typedef float RGBA[4];

#endif

#endif
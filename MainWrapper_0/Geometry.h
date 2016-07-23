#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "DirectX11.h"

struct MatrixBufferType
{
	mat world;
	mat view;
	mat projection;
};
struct VERTEX
{
	FLOAT X, Y, Z;
	color Color;
};
struct VS_INPUT
{
	vec4 Position;
	vec3 Normal;
	vec2 TextureCoordinate;
	mat BlendWeights;
	mat BlendIndices;
};
struct VS_OUTPUT
{
	vec4 Position;	
	vec2 TextureCoordinate;
	vec3 Normal;
};

extern D3D11_INPUT_ELEMENT_DESC ied[];
extern D3D11_INPUT_ELEMENT_DESC ied_VS_INPUT[];
extern D3D11_INPUT_ELEMENT_DESC ied_VS_OUTPUT[];

extern mat mIdentity, mTemp, mTemp2, mWorld, mView, mProj;
extern vec3 origin;

template <typename T> HRESULT FillBuffer(ID3D11Device *dev, ID3D11DeviceContext *devcon, ID3D11Buffer **ppOut, T pIn, UINT vSize)
{
	HRESULT hr;

	D3D11_MAPPED_SUBRESOURCE ms;
	hr = devcon->Map(*ppOut, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
	if (FAILED(hr))
		return hr;
	memcpy(ms.pData, pIn, vSize);
	devcon->Unmap(*ppOut, NULL);

	return S_OK;
}
mat TransposeMatrix(mat mIn);






#endif
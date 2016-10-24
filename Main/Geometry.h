#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "DirectX.h"

extern IDXGISwapChain *swapchain;
extern ID3D11Device *dev;
extern ID3D11DeviceContext *devcon;

extern ID3D11RenderTargetView *targettview;
extern ID3D11DepthStencilView *depthstencilview;

extern ID3D11RasterizerState *rasterizerstate;
extern ID3D11DepthStencilState *dss_enabled, *dss_disabled;

extern ID3D11VertexShader *vs_main, *vs_debug;
extern ID3D11PixelShader *ps_main, *ps_debug;
extern ID3D11InputLayout *il_main, *il_debug;

extern ID3D11Buffer *vertexbuffer, *indexbuffer, *constantbuffer;

extern mat mat_identity, mat_temp, mat_temp2, mat_world, mat_view, mat_proj;
extern float3  v_origin;

extern D3D11_INPUT_ELEMENT_DESC ied_main[], ied_debug[];

struct ConstantBuffer
{
	mat world;
	mat view;
	mat projection;
	color diffuse;
};
struct VERTEX_BASIC
{
	FLOAT X, Y, Z;
	color Color;
};
struct VERTEX
{
	float3 Position;
	float3 Normal;
	float2 TextureCoordinate;
};
struct VS_INPUT
{
	float4 Position;
	float3 Normal;
	float2 TextureCoordinate;
	mat BlendWeights;
	mat BlendIndices;
};
struct VS_OUTPUT
{
	float4 Position;
	float2 TextureCoordinate;
	float3 Normal;
};

template <typename T> HRESULT FillBuffer(ID3D11Device *dev, ID3D11DeviceContext *devcon, ID3D11Buffer **out, T in, UINT size)
{
	HRESULT hr;

	D3D11_MAPPED_SUBRESOURCE ms;
	hr = devcon->Map(*out, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
	if (FAILED(hr))
		return hr;
	memcpy(ms.pData, in, size);
	devcon->Unmap(*out, NULL);

	return S_OK;
}
mat TransposeMatrix(const mat &mIn);




#endif
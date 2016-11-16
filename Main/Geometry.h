#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "DirectX.h"
#include "SmartRelease.h"

extern IDXGISwapChain *swapchain;
extern ID3D11Device *dev;
extern ID3D11DeviceContext *devcon;

extern ID3D11RenderTargetView *targettview;
extern ID3D11DepthStencilView *depthstencilview;

extern ID3D11RasterizerState *rasterizerstate;
extern ID3D11BlendState *blendstate;
extern ID3D11DepthStencilState *dss_enabled, *dss_disabled;

extern ID3D11Buffer *vertexbuffer, *indexbuffer, *constantbuffer;

struct SHADER
{
	bool ready;

	ID3D11VertexShader *vs;
	ID3D11PixelShader *ps;
	ID3D11InputLayout *il;

	SHADER()
	{
		ready = false;
	}
	~SHADER()
	{
		smartRelease(vs);
		smartRelease(ps);
		smartRelease(il);
	}
};

extern SHADER sh_main, sh_debug, sh_plain;

#define SHADERS_MAIN &sh_main
#define SHADERS_DEBUG &sh_debug
#define SHADERS_PLAIN &sh_plain

extern mat mat_identity, mat_temp, mat_temp2, mat_world, mat_view, mat_proj, mat_orthoview, mat_orthoproj;
extern float2  v2_origin;
extern float3  v3_origin;

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

bool CompileShader(HRESULT *hr, std::wstring shader, SHADER *sh);
bool SetShader(SHADER *sh, ID3D11DeviceContext *devc = devcon);

HRESULT SetView(mat *world, mat *view, mat *proj, color diffuse = COLOR_WHITE, ID3D11Device *dv = dev, ID3D11DeviceContext *devc = devcon, ID3D11Buffer *cb = constantbuffer);

void Draw2DLineThin(float2 p1, float2 p2, color c1, color c2, color diffuse = COLOR_WHITE, ID3D11Device *dv = dev, ID3D11DeviceContext *devc = devcon, ID3D11Buffer *vb = vertexbuffer, ID3D11Buffer *ib = indexbuffer, ID3D11Buffer *cb = constantbuffer);
void Draw2DLineThick(float2 p1, float2 p2, float t, color c1, color c2, color diffuse = COLOR_WHITE, ID3D11Device *dv = dev, ID3D11DeviceContext *devc = devcon, ID3D11Buffer *vb = vertexbuffer, ID3D11Buffer *ib = indexbuffer, ID3D11Buffer *cb = constantbuffer);
void Draw2DRectangle(float w, float h, float x, float y, color c, color diffuse = COLOR_WHITE, ID3D11Device *dv = dev, ID3D11DeviceContext *devc = devcon, ID3D11Buffer *vb = vertexbuffer, ID3D11Buffer *ib = indexbuffer, ID3D11Buffer *cb = constantbuffer);
void Draw2DRectBorderThick(float w, float h, float x, float y, float t, color c, color diffuse = COLOR_WHITE, ID3D11Device *dv = dev, ID3D11DeviceContext *devc = devcon, ID3D11Buffer *vb = vertexbuffer, ID3D11Buffer *ib = indexbuffer, ID3D11Buffer *cb = constantbuffer);
void Draw2DFullRect(float w, float h, float x, float y, float t, color c1, color c2, color diffuse = COLOR_WHITE, ID3D11Device *dv = dev, ID3D11DeviceContext *devc = devcon, ID3D11Buffer *vb = vertexbuffer, ID3D11Buffer *ib = indexbuffer, ID3D11Buffer *cb = constantbuffer);
void Draw2DEllipses(float w, float h, float x, float y, color c, color diffuse = COLOR_WHITE, ID3D11Device *dv = dev, ID3D11DeviceContext *devc = devcon, ID3D11Buffer *vb = vertexbuffer, ID3D11Buffer *ib = indexbuffer, ID3D11Buffer *cb = constantbuffer);

void Draw3DLineThin(float3 p1, float3 p2, color c1, color c2, mat *world = &mat_world, color diffuse = COLOR_WHITE, ID3D11Device *dv = dev, ID3D11DeviceContext *devc = devcon, ID3D11Buffer *vb = vertexbuffer, ID3D11Buffer *ib = indexbuffer, ID3D11Buffer *cb = constantbuffer);
void Draw3DLineThick();
void Draw3DTriangle(float3 p1, float3 p2, float3 p3, color c, bool dd = false, mat *world = &mat_world, color diffuse = COLOR_WHITE, ID3D11Device *dv = dev, ID3D11DeviceContext *devc = devcon, ID3D11Buffer *vb = vertexbuffer, ID3D11Buffer *ib = indexbuffer, ID3D11Buffer *cb = constantbuffer);
void Draw3DRectangle(float w, float h, color c, bool dd = false, mat *world = &mat_world, color diffuse = COLOR_WHITE, ID3D11Device *dv = dev, ID3D11DeviceContext *devc = devcon, ID3D11Buffer *vb = vertexbuffer, ID3D11Buffer *ib = indexbuffer, ID3D11Buffer *cb = constantbuffer);
void Draw3DEllipses(float w, float h, color c, bool dd = false, mat *world = &mat_world, color diffuse = COLOR_WHITE, ID3D11Device *dv = dev, ID3D11DeviceContext *devc = devcon, ID3D11Buffer *vb = vertexbuffer, ID3D11Buffer *ib = indexbuffer, ID3D11Buffer *cb = constantbuffer);
void Draw3DBox(float w, float h, float b, color c, mat *world = &mat_world, color diffuse = COLOR_WHITE, ID3D11Device *dv = dev, ID3D11DeviceContext *devc = devcon, ID3D11Buffer *vb = vertexbuffer, ID3D11Buffer *ib = indexbuffer, ID3D11Buffer *cb = constantbuffer);
void Draw3DCube(float r, color c, mat *world = &mat_world, color diffuse = COLOR_WHITE, ID3D11Device *dv = dev, ID3D11DeviceContext *devc = devcon, ID3D11Buffer *vb = vertexbuffer, ID3D11Buffer *ib = indexbuffer, ID3D11Buffer *cb = constantbuffer);


#endif
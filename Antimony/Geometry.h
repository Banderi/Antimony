#pragma once

#include <vector>

#include "Warnings.h"
#include "DirectX.h"
#include "SmartRelease.h"
#include "Hresult.h"
#include "Param.h"

///

#define SHADERS_MAIN &sh_main
#define SHADERS_DEBUG &sh_debug
#define SHADERS_PLAIN &sh_plain
#define SHADERS_PLAIN3D &sh_plain3D

#define ON true
#define OFF false

///

extern IDXGISwapChain *swapchain;
extern ID3D11Device *dev;
extern ID3D11DeviceContext *devcon;

extern ID3D11RenderTargetView *targettview;
extern ID3D11DepthStencilView *depthstencilview;

extern ID3D11RasterizerState *rss_standard, *rss_wireframe;
extern ID3D11BlendState *blendstate;
extern ID3D11DepthStencilState *dss_enabled, *dss_disabled;

extern ID3D11Buffer *indexbuffer, *constantbuffer;

extern mat mat_identity, mat_temp, mat_temp2, mat_world, mat_view, mat_proj, mat_orthoview, mat_orthoproj;
extern float2 v2_origin;
extern float3 v3_origin;

extern D3D11_INPUT_ELEMENT_DESC ied_basic[], ied_basic_skinned[], ied_main[], ied_main_skinned[];

///

struct ConstantBuffer
{
	mat world;
	mat view;
	mat projection;
	mat normal;
	color diffuse;
	float4 camera;
};
struct VERTEX_BASIC
{
	float3 position;
	color color;
};
struct VERTEX_BASIC_SKINNED
{
	FLOAT x, y, z;
	color color;
};
struct VERTEX_MAIN
{
	float3 position;
	float3 normal;
	float2 uv;
};
struct VERTEX_MAIN_SKINNED
{
	float3 position;
	float3 normal;
	float2 uv;
};

struct cursor
{
	float x, y;
	ID3D11Texture2D *icon;
};

struct SHADER
{
	bool ready;

	ID3D11VertexShader *vs;
	ID3D11PixelShader *ps;
	ID3D11InputLayout *il;

	ID3D11GeometryShader *gs;
	ID3D11HullShader *hs;
	ID3D11ComputeShader *cs;
	ID3D11DomainShader *ds;

	ID3D11Buffer *vb;
	UINT vbstride;
	UINT vbsize;
	UINT vboffset;

	D3D11_INPUT_ELEMENT_DESC* ied;

	SHADER()
	{
		vbstride = 0;
		vboffset = 0;
		ready = false;
	}
	~SHADER()
	{
		smartRelease(vs);
		smartRelease(ps);
		smartRelease(il);
	}
};

extern SHADER sh_main, sh_debug, sh_plain, sh_plain3D;
extern SHADER *sh_current;

class VertexCompound
{
private:
	UINT *arrIndices;
	VERTEX_BASIC *arrBASIC;
	VERTEX_BASIC_SKINNED *arrBASIC_SKINNED;
	VERTEX_MAIN *arrMAIN;
	VERTEX_MAIN_SKINNED *arrMAIN_SKINNED;

public:
	std::vector<UINT> index;
	std::vector<float3> position;
	std::vector<float3> normal;
	std::vector<std::vector<float3>> normalgroups;
	std::vector<float2> uv;
	std::vector<color> col;

	UINT* dumpIndices();
	VERTEX_BASIC* dumpBASIC();
	VERTEX_BASIC_SKINNED* dumpBASIC_SKINNED();
	VERTEX_MAIN* dumpMAIN();
	VERTEX_MAIN_SKINNED* dumpMAIN_SKINNED();

	void destroyDumps();

	VertexCompound()
	{
		arrIndices = nullptr;
		arrBASIC = nullptr;
		arrBASIC_SKINNED = nullptr;
		arrMAIN = nullptr;
		arrMAIN_SKINNED = nullptr;
	}
	~VertexCompound()
	{
		destroyDumps();
	}
};

///

unsigned long RGBA2DWORD(int iR, int iG, int iB, int iA);

//double CubicInterpolate(double y0, double y1, double y2, double y3, double mu);
//float3 CubicInterpolate(float3 y0, float3 y1, float3 y2, float3 y3, double mu);
template<typename T> T CubicInterpolate(T y0, T y1, T y2, T y3, double mu);

mat TransposeMatrix(const mat &mIn);
float3 MatToFloat3(mat *m);

float3 WorldToScreen(float3 p, mat *viewproj = &(mat_view * mat_proj), float2 screen = float2(Antimony::display.width, Antimony::display.height));
mat HingeBillboard(float3 p1, float3 p2, float3 pos);
mat FreeBillboard(float3 cam, float3 pos, float3 up, mat *view);

void CalculateSmoothNormals(VERTEX_MAIN vin[], UINT vcount, UINT iin[], UINT icount, std::vector<std::vector<float3>> *normalgroups, bool surf_weighting = false, bool ang_weighting = true);

//template <typename T> HRESULT Antimony::FillBuffer(ID3D11Device *dev, ID3D11DeviceContext *devcon, ID3D11Buffer **out, T in, UINT size)
//{
//	HRESULT hr;
//
//	D3D11_MAPPED_SUBRESOURCE ms;
//	hr = devcon->Map(*out, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
//	if (!handleErr(&hr, HRH_BUFFER_MAPPING, hr))
//		return hr;
//	D3D11_BUFFER_DESC bd;
//	(*out)->GetDesc(&bd);
//	UINT buffsize = bd.ByteWidth;
//	if (size > buffsize)
//	{
//		handleErr(&hr, HRH_BUFFER_OVERFLOW, DISP_E_BUFFERTOOSMALL);
//		return DISP_E_BUFFERTOOSMALL;
//	}
//	memcpy(ms.pData, in, size);
//	devcon->Unmap(*out, NULL);
//
//	return S_OK;
//}

// TODO: Make drawing functions independent from shaders

void Draw2DDot(float2 p, float t, color c, color diffuse = COLOR_WHITE, ID3D11Device *dv = dev, ID3D11DeviceContext *devc = devcon, SHADER* sh = sh_current, ID3D11Buffer *ib = indexbuffer, ID3D11Buffer *cb = constantbuffer);
void Draw2DLineThin(float2 p1, float2 p2, color c1, color c2, color diffuse = COLOR_WHITE, ID3D11Device *dv = dev, ID3D11DeviceContext *devc = devcon, SHADER* sh = sh_current, ID3D11Buffer *ib = indexbuffer, ID3D11Buffer *cb = constantbuffer);
void Draw2DLineThick(float2 p1, float2 p2, float t, color c1, color c2, color diffuse = COLOR_WHITE, ID3D11Device *dv = dev, ID3D11DeviceContext *devc = devcon, SHADER* sh = sh_current, ID3D11Buffer *ib = indexbuffer, ID3D11Buffer *cb = constantbuffer);
void Draw2DRectangle(float w, float h, float x, float y, color c, color diffuse = COLOR_WHITE, ID3D11Device *dv = dev, ID3D11DeviceContext *devc = devcon, SHADER* sh = sh_current, ID3D11Buffer *ib = indexbuffer, ID3D11Buffer *cb = constantbuffer);
void Draw2DRectBorderThick(float w, float h, float x, float y, float t, color c, color diffuse = COLOR_WHITE, ID3D11Device *dv = dev, ID3D11DeviceContext *devc = devcon, SHADER* sh = sh_current, ID3D11Buffer *ib = indexbuffer, ID3D11Buffer *cb = constantbuffer);
void Draw2DFullRect(float w, float h, float x, float y, float t, color c1, color c2, color diffuse = COLOR_WHITE, ID3D11Device *dv = dev, ID3D11DeviceContext *devc = devcon, SHADER* sh = sh_current, ID3D11Buffer *ib = indexbuffer, ID3D11Buffer *cb = constantbuffer);
void Draw2DEllipses(float w, float h, float x, float y, color c, color diffuse = COLOR_WHITE, ID3D11Device *dv = dev, ID3D11DeviceContext *devc = devcon, SHADER* sh = sh_current, ID3D11Buffer *ib = indexbuffer, ID3D11Buffer *cb = constantbuffer);

void Draw3DLineThin(float3 p1, float3 p2, color c1, color c2, mat *mat_world = &::mat_world, color diffuse = COLOR_WHITE, ID3D11Device *dv = dev, ID3D11DeviceContext *devc = devcon, SHADER* sh = sh_current, ID3D11Buffer *ib = indexbuffer, ID3D11Buffer *cb = constantbuffer);
void Draw3DLineThick();
void Draw3DTriangle(float3 p1, float3 p2, float3 p3, color c, bool dd = false, mat *mat_world = &::mat_world, color diffuse = COLOR_WHITE, ID3D11Device *dv = dev, ID3D11DeviceContext *devc = devcon, SHADER* sh = sh_current, ID3D11Buffer *ib = indexbuffer, ID3D11Buffer *cb = constantbuffer);
void Draw3DRectangle(float w, float h, color c, bool dd = false, mat *mat_world = &::mat_world, color diffuse = COLOR_WHITE, ID3D11Device *dv = dev, ID3D11DeviceContext *devc = devcon, SHADER* sh = sh_current, ID3D11Buffer *ib = indexbuffer, ID3D11Buffer *cb = constantbuffer);
void Draw3DEllipses(float w, float h, color c, bool dd = false, mat *mat_world = &::mat_world, color diffuse = COLOR_WHITE, ID3D11Device *dv = dev, ID3D11DeviceContext *devc = devcon, SHADER* sh = sh_current, ID3D11Buffer *ib = indexbuffer, ID3D11Buffer *cb = constantbuffer);
void Draw3DBox(float w, float h, float b, color c, mat *mat_world = &::mat_world, color diffuse = COLOR_WHITE, ID3D11Device *dv = dev, ID3D11DeviceContext *devc = devcon, SHADER* sh = sh_current, ID3D11Buffer *ib = indexbuffer, ID3D11Buffer *cb = constantbuffer);
void Draw3DBox(Vector3 l, color c, mat *mat_world = &::mat_world, color diffuse = COLOR_WHITE, ID3D11Device *dv = dev, ID3D11DeviceContext *devc = devcon, SHADER* sh = sh_current, ID3D11Buffer *ib = indexbuffer, ID3D11Buffer *cb = constantbuffer);
void Draw3DCube(float r, color c, mat *mat_world = &::mat_world, color diffuse = COLOR_WHITE, ID3D11Device *dv = dev, ID3D11DeviceContext *devc = devcon, SHADER* sh = sh_current, ID3D11Buffer *ib = indexbuffer, ID3D11Buffer *cb = constantbuffer);

void DrawMesh(VertexCompound *mesh, mat *mat_world = &::mat_world, color diffuse = COLOR_WHITE, ID3D11Device *dv = dev, ID3D11DeviceContext *devc = devcon, SHADER* sh = sh_current, ID3D11Buffer *ib = indexbuffer, ID3D11Buffer *cb = constantbuffer);

namespace Antimony
{
	HRESULT FillBuffer(ID3D11Device *dev, ID3D11DeviceContext *devcon, ID3D11Buffer **out, void *in, UINT size);
	bool compileShader(HRESULT *hr, std::wstring shader, SHADER *sh, D3D11_INPUT_ELEMENT_DESC ied[], UINT elems, UINT stride, UINT size = 512);
	bool setShader(SHADER *sh, ID3D11DeviceContext *devc = devcon);
	void setDepthBufferState(bool state);
	HRESULT setView(mat *mat_world, mat *view, mat *proj, color diffuse = COLOR_WHITE, ID3D11Device *dv = dev, ID3D11DeviceContext *devc = devcon, ID3D11Buffer *cb = constantbuffer);
}
#pragma once

#include <vector>

#include "Warnings.h"
#include "DirectX.h"
#include "SmartRelease.h"
#include "Hresult.h"
#include "Param.h"

///

#define SHADERS_MAIN &Antimony::sh_main
#define SHADERS_DEBUG &Antimony::sh_debug
#define SHADERS_PLAIN &Antimony::sh_plain
#define SHADERS_PLAIN3D &Antimony::sh_plain3D

#define ON true
#define OFF false

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

struct Shader
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

	Shader()
	{
		vbstride = 0;
		vboffset = 0;
		ready = false;
	}
	~Shader()
	{
		smartRelease(vs);
		smartRelease(ps);
		smartRelease(il);
	}
};

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

extern mat mat_identity;
extern float2 v2_origin;
extern float3 v3_origin;

///

unsigned long RGBA2DWORD(int iR, int iG, int iB, int iA);

mat TransposeMatrix(const mat &mIn);
float3 MatToFloat3(mat *m);

float3 WorldToScreen(float3 p);
mat HingeBillboard(float3 p1, float3 p2, float3 pos);
mat FreeBillboard(float3 cam, float3 pos, float3 up, mat *view);

void CalculateSmoothNormals(VERTEX_MAIN vin[], UINT vcount, UINT iin[], UINT icount, std::vector<std::vector<float3>> *normalgroups, bool surf_weighting = false, bool ang_weighting = true);

namespace Antimony
{
	extern IDXGISwapChain *swapchain;
	extern ID3D11Device *dev;
	extern ID3D11DeviceContext *devcon;

	extern ID3D11RenderTargetView *targettview;
	extern ID3D11DepthStencilView *depthstencilview;

	extern ID3D11RasterizerState *rss_standard, *rss_wireframe;
	extern ID3D11BlendState *blendstate;
	extern ID3D11DepthStencilState *dss_enabled, *dss_disabled;

	extern ID3D11Buffer *indexbuffer, *constantbuffer;

	extern Shader sh_main, sh_debug, sh_plain, sh_plain3D;
	extern Shader *sh_current;

	extern mat mat_temp, mat_temp2, mat_world, mat_view, mat_proj, mat_orthoview, mat_orthoproj;

	extern D3D11_INPUT_ELEMENT_DESC ied_basic[], ied_basic_skinned[], ied_main[], ied_main_skinned[];

	HRESULT FillBuffer(ID3D11Device *dev, ID3D11DeviceContext *devcon, ID3D11Buffer **out, void *in, UINT size);
	bool compileShader(HRESULT *hr, ::std::wstring shader, Shader *sh, D3D11_INPUT_ELEMENT_DESC ied[], UINT elems, UINT stride, UINT size = 512);
	bool setShader(Shader *sh, ID3D11DeviceContext *devc = devcon);
	void setDepthBufferState(bool state);
	HRESULT setView(mat *m_world, mat *view, mat *proj, color diffuse = COLOR_WHITE, ID3D11Device *dv = dev, ID3D11DeviceContext *devc = devcon, ID3D11Buffer *cb = constantbuffer);

	// TODO: Make drawing functions independent from shaders

	void Draw2DDot(float2 p, float t, color c, color diffuse = COLOR_WHITE);
	void Draw2DLineThin(float2 p1, float2 p2, color c1, color c2, color diffuse = COLOR_WHITE);
	void Draw2DLineThick(float2 p1, float2 p2, float t, color c1, color c2, color diffuse = COLOR_WHITE);
	void Draw2DRectangle(float w, float h, float x, float y, color c, color diffuse = COLOR_WHITE);
	void Draw2DRectBorderThick(float w, float h, float x, float y, float t, color c, color diffuse = COLOR_WHITE);
	void Draw2DFullRect(float w, float h, float x, float y, float t, color c1, color c2, color diffuse = COLOR_WHITE);
	void Draw2DEllipses(float w, float h, float x, float y, color c, color diffuse = COLOR_WHITE);

	void Draw3DLineThin(float3 p1, float3 p2, color c1, color c2, mat *m_world = &mat_world, color diffuse = COLOR_WHITE);
	void Draw3DLineThick();
	void Draw3DTriangle(float3 p1, float3 p2, float3 p3, color c, bool dd = false, mat *m_world = &mat_world, color diffuse = COLOR_WHITE);
	void Draw3DRectangle(float w, float h, color c, bool dd = false, mat *m_world = &mat_world, color diffuse = COLOR_WHITE);
	void Draw3DEllipses(float w, float h, color c, float sec = 2.0f * MATH_PI, bool dd = false, mat *m_world = &mat_world, color diffuse = COLOR_WHITE);
	void DrawCylinderSide(float r, float h, color c, float sec = 2.0f * MATH_PI, bool dd = false, mat *m_world = &mat_world, color diffuse = COLOR_WHITE);
	void DrawCylinder(float r, float h, color c, float sec = 2.0f * MATH_PI, bool dd = false, mat *m_world = &mat_world, color diffuse = COLOR_WHITE);

	void Draw3DBox(float w, float h, float b, color c, mat *m_world = &mat_world, color diffuse = COLOR_WHITE);
	void Draw3DBox(float3 l, color c, mat *m_world = &mat_world, color diffuse = COLOR_WHITE);
	void Draw3DCube(float r, color c, mat *m_world = &mat_world, color diffuse = COLOR_WHITE);

	void DrawMeshPlain(VertexCompound *mesh, mat *m_world = &mat_world, color diffuse = COLOR_WHITE);
	void DrawMesh(VertexCompound *mesh, mat *m_world = &mat_world, color diffuse = COLOR_WHITE);
	void DrawNormals(VERTEX_MAIN vin[], int vcount, ::std::vector<::std::vector<float3>> *normalgroups, bool components = false);
}
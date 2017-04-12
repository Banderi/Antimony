#include "Warnings.h"
#include "Geometry.h"
#include "Hresult.h"

///

IDXGISwapChain *swapchain;
ID3D11Device *dev;
ID3D11DeviceContext *devcon;

ID3D11RenderTargetView *targettview;
ID3D11DepthStencilView *depthstencilview;

ID3D11RasterizerState *rasterizerstate;
ID3D11BlendState *blendstate;
ID3D11DepthStencilState *dss_enabled, *dss_disabled;

ID3D11Buffer *vertexbuffer, *indexbuffer, *constantbuffer;

SHADER sh_main, sh_debug, sh_plain;

mat mat_identity, mat_temp, mat_temp2, mat_world, mat_view, mat_proj, mat_orthoview, mat_orthoproj;
float2  v2_origin = float2(0, 0);
float3  v3_origin = float3(0, 0, 0);

UINT vertex_stride = sizeof(VERTEX_BASIC);
UINT vertex_offset = 0;

D3D11_INPUT_ELEMENT_DESC ied_debug[] = {
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};
D3D11_INPUT_ELEMENT_DESC ied_main[] = {
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};
D3D11_INPUT_ELEMENT_DESC ied_VS_INPUT[] = {
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "BLENDINDICES", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};
D3D11_INPUT_ELEMENT_DESC ied_VS_OUTPUT[] = {
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

///

mat TransposeMatrix(const mat &mIn)
{
	mat mat_temp;
	mat_temp = MTranspose(mIn);
	return mat_temp;
}
float3 MatToFloat3(mat *m)
{
	XMFLOAT4X4 f;
	XMStoreFloat4x4(&f, *m);
	return float3(f._41, f._42, f._43);
}

bool setShader(SHADER *sh, ID3D11DeviceContext *devc)
{
	if (sh->ready)
	{
		devc->VSSetShader(sh->vs, 0, 0);
		devc->PSSetShader(sh->ps, 0, 0);
		devc->IASetInputLayout(sh->il);
		devc->GSSetShader(sh->gs, 0, 0);
		devc->HSSetShader(sh->hs, 0, 0);
		devc->CSSetShader(sh->cs, 0, 0);
		devc->DSSetShader(sh->ds, 0, 0);
		return 1; // all ok
	}
	return 0; // NOPE
}
void setDepthBufferState(bool state)
{
	if (state == true)
		devcon->OMSetDepthStencilState(dss_enabled, 1);
	else
		devcon->OMSetDepthStencilState(dss_disabled, 1);
}
HRESULT setView(mat *world, mat *view, mat *proj, color diffuse, ID3D11Device *dv, ID3D11DeviceContext *devc, ID3D11Buffer *cb)
{
	ConstantBuffer buffer;

	buffer.world = TransposeMatrix(*world);
	buffer.view = TransposeMatrix(*view);
	buffer.projection = TransposeMatrix(*proj);
	buffer.diffuse = diffuse;

	return FillBuffer(dv, devc, &cb, &buffer, sizeof(buffer));
}

void Draw2DLineThin(float2 p1, float2 p2, color c1, color c2, color diffuse, ID3D11Device *dv, ID3D11DeviceContext *devc, ID3D11Buffer *vb, ID3D11Buffer *ib, ID3D11Buffer *cb)
{
	p1.y = -p1.y;
	p2.y = -p2.y;
	VERTEX_BASIC vertices[] =
	{
		{ p1.x, p1.y, 0, c1 },
		{ p2.x, p2.y, 0, c2 }
	};
	FillBuffer<VERTEX_BASIC[]>(dv, devc, &vb, vertices, sizeof(vertices));

	setView(&mat_identity, &mat_orthoview, &mat_orthoproj, diffuse, dv, devc, cb);
	devc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	devc->Draw(2, 0);
}
void Draw2DLineThick(float2 p1, float2 p2, float t, color c1, color c2, color diffuse, ID3D11Device *dv, ID3D11DeviceContext *devc, ID3D11Buffer *vb, ID3D11Buffer *ib, ID3D11Buffer *cb)
{
	p1.y = -p1.y;
	p2.y = -p2.y;
	float dist = *XMVector2Length(p1 - p2).m128_f32;

	VERTEX_BASIC vertices[] =
	{
		{ 0, -0.5 * t, 0, c1 },
		{ 0, 0.5 * t, 0, c1 },
		{ dist, -0.5 * t, 0, c2 },
		{ dist, 0.5 * t, 0, c2 }
	};
	FillBuffer<VERTEX_BASIC[]>(dv, devc, &vb, vertices, sizeof(vertices));
	UINT indices[] =
	{
		0, 3, 2,
		0, 1, 3
	};
	FillBuffer<UINT[]>(dv, devc, &ib, indices, sizeof(indices));

	setView(&(MRotZ(atan2(p2.y - p1.y, p2.x - p1.x)) * MTranslation(p1.x, p1.y, 0)), &mat_orthoview, &mat_orthoproj, diffuse, dv, devc, cb);
	devc->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	devc->DrawIndexed(6, 0, 0);
}
void Draw2DRectangle(float w, float h, float x, float y, color c, color diffuse, ID3D11Device *dv, ID3D11DeviceContext *devc, ID3D11Buffer *vb, ID3D11Buffer *ib, ID3D11Buffer *cb)
{
	y = -y;
	VERTEX_BASIC vertices[] =
	{
		{ 0, 0, 0, c },
		{ w, 0, 0, c },
		{ w, h, 0, c },
		{ 0, h, 0, c }
	};
	FillBuffer<VERTEX_BASIC[]>(dv, devc, &vb, vertices, sizeof(vertices));
	UINT indices[] =
	{
		0, 2, 1,
		0, 3, 2
	};
	FillBuffer<UINT[]>(dv, devc, &ib, indices, sizeof(indices));

	setView(&MTranslation(x, y, 0), &mat_orthoview, &mat_orthoproj, diffuse, dv, devc, cb);
	devc->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	devc->DrawIndexed(6, 0, 0);
}
void Draw2DRectBorderThick(float w, float h, float x, float y, float t, color c, color diffuse, ID3D11Device *dv, ID3D11DeviceContext *devc, ID3D11Buffer *vb, ID3D11Buffer *ib, ID3D11Buffer *cb)
{
	h = -h;
	Draw2DLineThick(float2(x - 0.5 * t, y), float2(x + w + 0.5 * t, y), t, c, c, diffuse, dv, devc, vb, ib, cb);
	Draw2DLineThick(float2(x + w, y + 0.5 * t), float2(x + w, y + h - 0.5 * t), t, c, c, diffuse, dv, devc, vb, ib, cb);
	Draw2DLineThick(float2(x + w + 0.5 * t, y + h), float2(x - 0.5 * t, y + h), t, c, c, diffuse, dv, devc, vb, ib, cb);
	Draw2DLineThick(float2(x, y + h - 0.5 * t), float2(x, y + 0.5 * t), t, c, c, diffuse, dv, devc, vb, ib, cb);
}
void Draw2DFullRect(float w, float h, float x, float y, float t, color c1, color c2, color diffuse, ID3D11Device *dv, ID3D11DeviceContext *devc, ID3D11Buffer *vb, ID3D11Buffer *ib, ID3D11Buffer *cb)
{
	Draw2DRectangle(w, h, x, y, c1, diffuse, dv, devc, vb, ib, cb);
	Draw2DRectBorderThick(w, h, x, y, t, c2, diffuse, dv, devc, vb, ib, cb);
}
void Draw2DEllipses(float w, float h, float x, float y, color c, color diffuse, ID3D11Device *dv, ID3D11DeviceContext *devc, ID3D11Buffer *vb, ID3D11Buffer *ib, ID3D11Buffer *cb)
{
	y = -y;
	VERTEX_BASIC vertices[] =
	{
		{ 0, 0, 0, c },
		{ cosf(0 * DX_PI / 20), sinf(0 * DX_PI / 20), 0, c },
		{ cosf(1 * DX_PI / 20), sinf(1 * DX_PI / 20), 0, c },
		{ cosf(2 * DX_PI / 20), sinf(2 * DX_PI / 20), 0, c },
		{ cosf(3 * DX_PI / 20), sinf(3 * DX_PI / 20), 0, c },
		{ cosf(4 * DX_PI / 20), sinf(4 * DX_PI / 20), 0, c },
		{ cosf(5 * DX_PI / 20), sinf(5 * DX_PI / 20), 0, c },
		{ cosf(6 * DX_PI / 20), sinf(6 * DX_PI / 20), 0, c },
		{ cosf(7 * DX_PI / 20), sinf(7 * DX_PI / 20), 0, c },
		{ cosf(8 * DX_PI / 20), sinf(8 * DX_PI / 20), 0, c },
		{ cosf(9 * DX_PI / 20), sinf(9 * DX_PI / 20), 0, c },
		{ cosf(10 * DX_PI / 20), sinf(10 * DX_PI / 20), 0, c },
		{ cosf(11 * DX_PI / 20), sinf(11 * DX_PI / 20), 0, c },
		{ cosf(12 * DX_PI / 20), sinf(12 * DX_PI / 20), 0, c },
		{ cosf(13 * DX_PI / 20), sinf(13 * DX_PI / 20), 0, c },
		{ cosf(14 * DX_PI / 20), sinf(14 * DX_PI / 20), 0, c },
		{ cosf(15 * DX_PI / 20), sinf(15 * DX_PI / 20), 0, c },
		{ cosf(16 * DX_PI / 20), sinf(16 * DX_PI / 20), 0, c },
		{ cosf(17 * DX_PI / 20), sinf(17 * DX_PI / 20), 0, c },
		{ cosf(18 * DX_PI / 20), sinf(18 * DX_PI / 20), 0, c },
		{ cosf(19 * DX_PI / 20), sinf(19 * DX_PI / 20), 0, c },
		{ cosf(20 * DX_PI / 20), sinf(20 * DX_PI / 20), 0, c },
		{ cosf(21 * DX_PI / 20), sinf(21 * DX_PI / 20), 0, c },
		{ cosf(22 * DX_PI / 20), sinf(22 * DX_PI / 20), 0, c },
		{ cosf(23 * DX_PI / 20), sinf(23 * DX_PI / 20), 0, c },
		{ cosf(24 * DX_PI / 20), sinf(24 * DX_PI / 20), 0, c },
		{ cosf(25 * DX_PI / 20), sinf(25 * DX_PI / 20), 0, c },
		{ cosf(26 * DX_PI / 20), sinf(26 * DX_PI / 20), 0, c },
		{ cosf(27 * DX_PI / 20), sinf(27 * DX_PI / 20), 0, c },
		{ cosf(28 * DX_PI / 20), sinf(28 * DX_PI / 20), 0, c },
		{ cosf(29 * DX_PI / 20), sinf(29 * DX_PI / 20), 0, c },
		{ cosf(30 * DX_PI / 20), sinf(30 * DX_PI / 20), 0, c },
		{ cosf(31 * DX_PI / 20), sinf(31 * DX_PI / 20), 0, c },
		{ cosf(32 * DX_PI / 20), sinf(32 * DX_PI / 20), 0, c },
		{ cosf(33 * DX_PI / 20), sinf(33 * DX_PI / 20), 0, c },
		{ cosf(34 * DX_PI / 20), sinf(34 * DX_PI / 20), 0, c },
		{ cosf(35 * DX_PI / 20), sinf(35 * DX_PI / 20), 0, c },
		{ cosf(36 * DX_PI / 20), sinf(36 * DX_PI / 20), 0, c },
		{ cosf(37 * DX_PI / 20), sinf(37 * DX_PI / 20), 0, c },
		{ cosf(38 * DX_PI / 20), sinf(38 * DX_PI / 20), 0, c },
		{ cosf(39 * DX_PI / 20), sinf(39 * DX_PI / 20), 0, c },
		{ cosf(40 * DX_PI / 20), sinf(40 * DX_PI / 20), 0, c }
	};
	FillBuffer<VERTEX_BASIC[]>(dv, devc, &vb, vertices, sizeof(vertices));
	UINT indices[] =
	{
		1, 2, 0,
		2, 3, 0,
		3, 4, 0,
		4, 5, 0,
		5, 6, 0,
		6, 7, 0,
		7, 8, 0,
		8, 9, 0,
		9, 10, 0,
		10, 11, 0,
		11, 12, 0,
		12, 13, 0,
		13, 14, 0,
		14, 15, 0,
		15, 16, 0,
		16, 17, 0,
		17, 18, 0,
		18, 19, 0,
		19, 20, 0,
		20, 21, 0,
		21, 22, 0,
		22, 23, 0,
		23, 24, 0,
		24, 25, 0,
		25, 26, 0,
		26, 27, 0,
		27, 28, 0,
		28, 29, 0,
		29, 30, 0,
		30, 31, 0,
		31, 32, 0,
		32, 33, 0,
		33, 34, 0,
		34, 35, 0,
		35, 36, 0,
		36, 37, 0,
		37, 38, 0,
		38, 39, 0,
		39, 40, 0,
		40, 1, 0
	};
	FillBuffer<UINT[]>(dv, devc, &ib, indices, sizeof(indices));

	setView(&(MScaling(-w, h, 1) * MTranslation(x, y, 0)), &mat_orthoview, &mat_orthoproj, diffuse, dv, devc, cb);
	devc->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	devc->DrawIndexed(120, 0, 0);
}

void Draw3DLineThin(float3 p1, float3 p2, color c1, color c2, mat *world, color diffuse, ID3D11Device *dv, ID3D11DeviceContext *devc, ID3D11Buffer *vb, ID3D11Buffer *ib, ID3D11Buffer *cb)
{
	VERTEX_BASIC vertices[] =
	{
		{ p1.x, p1.y, p1.z, c1 },
		{ p2.x, p2.y, p2.z, c2 }
	};
	FillBuffer<VERTEX_BASIC[]>(dv, devc, &vb, vertices, sizeof(vertices));

	setView(world, &mat_view, &mat_proj, diffuse, dv, devc, cb);
	devc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	devc->Draw(2, 0);
}
void Draw3DLineThick()
{
	// TODO: Implement bill-boarding
}
void Draw3DTriangle(float3 p1, float3 p2, float3 p3, color c, bool dd, mat *world, color diffuse, ID3D11Device *dv, ID3D11DeviceContext *devc, ID3D11Buffer *vb, ID3D11Buffer *ib, ID3D11Buffer *cb)
{
	VERTEX_BASIC vertices[] =
	{
		{ p1.x, p1.y, p1.z, c },
		{ p2.x, p2.y, p2.z, c },
		{ p3.x, p3.y, p3.z, c }
	};
	FillBuffer<VERTEX_BASIC[]>(dv, devc, &vb, vertices, sizeof(vertices));
	UINT indices[] =
	{
		0, 2, 1,
		0, 1, 2
	};
	FillBuffer<UINT[]>(dv, devc, &ib, indices, sizeof(indices));

	setView(world, &mat_view, &mat_proj, diffuse, dv, devc, cb);
	devc->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	if (!dd)
		devc->DrawIndexed(3, 0, 0);
	else
		devc->DrawIndexed(6, 0, 0);
}
void Draw3DRectangle(float w, float h, color c, bool dd, mat *world, color diffuse, ID3D11Device *dv, ID3D11DeviceContext *devc, ID3D11Buffer *vb, ID3D11Buffer *ib, ID3D11Buffer *cb)
{
	VERTEX_BASIC vertices[] =
	{
		{ 0, 0, 0, c },
		{ w, 0, 0, c },
		{ w, h, 0, c },
		{ 0, h, 0, c }
	};
	FillBuffer<VERTEX_BASIC[]>(dv, devc, &vb, vertices, sizeof(vertices));
	UINT indices[] =
	{
		0, 2, 1,
		0, 3, 2,
		0, 1, 2,
		0, 2, 3
	};
	FillBuffer<UINT[]>(dv, devc, &ib, indices, sizeof(indices));

	setView(world, &mat_view, &mat_proj, diffuse, dv, devc, cb);
	devc->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	if (!dd)
		devc->DrawIndexed(6, 0, 0);
	else
		devc->DrawIndexed(12, 0, 0);
}
void Draw3DEllipses(float w, float h, color c, bool dd, mat *world, color diffuse, ID3D11Device *dv, ID3D11DeviceContext *devc, ID3D11Buffer *vb, ID3D11Buffer *ib, ID3D11Buffer *cb)
{
	VERTEX_BASIC vertices[] =
	{
		{ 0, 0, 0, c },
		{ cosf(0 * DX_PI / 20), sinf(0 * DX_PI / 20), 0, c },
		{ cosf(1 * DX_PI / 20), sinf(1 * DX_PI / 20), 0, c },
		{ cosf(2 * DX_PI / 20), sinf(2 * DX_PI / 20), 0, c },
		{ cosf(3 * DX_PI / 20), sinf(3 * DX_PI / 20), 0, c },
		{ cosf(4 * DX_PI / 20), sinf(4 * DX_PI / 20), 0, c },
		{ cosf(5 * DX_PI / 20), sinf(5 * DX_PI / 20), 0, c },
		{ cosf(6 * DX_PI / 20), sinf(6 * DX_PI / 20), 0, c },
		{ cosf(7 * DX_PI / 20), sinf(7 * DX_PI / 20), 0, c },
		{ cosf(8 * DX_PI / 20), sinf(8 * DX_PI / 20), 0, c },
		{ cosf(9 * DX_PI / 20), sinf(9 * DX_PI / 20), 0, c },
		{ cosf(10 * DX_PI / 20), sinf(10 * DX_PI / 20), 0, c },
		{ cosf(11 * DX_PI / 20), sinf(11 * DX_PI / 20), 0, c },
		{ cosf(12 * DX_PI / 20), sinf(12 * DX_PI / 20), 0, c },
		{ cosf(13 * DX_PI / 20), sinf(13 * DX_PI / 20), 0, c },
		{ cosf(14 * DX_PI / 20), sinf(14 * DX_PI / 20), 0, c },
		{ cosf(15 * DX_PI / 20), sinf(15 * DX_PI / 20), 0, c },
		{ cosf(16 * DX_PI / 20), sinf(16 * DX_PI / 20), 0, c },
		{ cosf(17 * DX_PI / 20), sinf(17 * DX_PI / 20), 0, c },
		{ cosf(18 * DX_PI / 20), sinf(18 * DX_PI / 20), 0, c },
		{ cosf(19 * DX_PI / 20), sinf(19 * DX_PI / 20), 0, c },
		{ cosf(20 * DX_PI / 20), sinf(20 * DX_PI / 20), 0, c },
		{ cosf(21 * DX_PI / 20), sinf(21 * DX_PI / 20), 0, c },
		{ cosf(22 * DX_PI / 20), sinf(22 * DX_PI / 20), 0, c },
		{ cosf(23 * DX_PI / 20), sinf(23 * DX_PI / 20), 0, c },
		{ cosf(24 * DX_PI / 20), sinf(24 * DX_PI / 20), 0, c },
		{ cosf(25 * DX_PI / 20), sinf(25 * DX_PI / 20), 0, c },
		{ cosf(26 * DX_PI / 20), sinf(26 * DX_PI / 20), 0, c },
		{ cosf(27 * DX_PI / 20), sinf(27 * DX_PI / 20), 0, c },
		{ cosf(28 * DX_PI / 20), sinf(28 * DX_PI / 20), 0, c },
		{ cosf(29 * DX_PI / 20), sinf(29 * DX_PI / 20), 0, c },
		{ cosf(30 * DX_PI / 20), sinf(30 * DX_PI / 20), 0, c },
		{ cosf(31 * DX_PI / 20), sinf(31 * DX_PI / 20), 0, c },
		{ cosf(32 * DX_PI / 20), sinf(32 * DX_PI / 20), 0, c },
		{ cosf(33 * DX_PI / 20), sinf(33 * DX_PI / 20), 0, c },
		{ cosf(34 * DX_PI / 20), sinf(34 * DX_PI / 20), 0, c },
		{ cosf(35 * DX_PI / 20), sinf(35 * DX_PI / 20), 0, c },
		{ cosf(36 * DX_PI / 20), sinf(36 * DX_PI / 20), 0, c },
		{ cosf(37 * DX_PI / 20), sinf(37 * DX_PI / 20), 0, c },
		{ cosf(38 * DX_PI / 20), sinf(38 * DX_PI / 20), 0, c },
		{ cosf(39 * DX_PI / 20), sinf(39 * DX_PI / 20), 0, c },
		{ cosf(40 * DX_PI / 20), sinf(40 * DX_PI / 20), 0, c }
	};
	FillBuffer<VERTEX_BASIC[]>(dv, devc, &vb, vertices, sizeof(vertices));
	UINT indices[] =
	{
		1, 2, 0,
		2, 3, 0,
		3, 4, 0,
		4, 5, 0,
		5, 6, 0,
		6, 7, 0,
		7, 8, 0,
		8, 9, 0,
		9, 10, 0,
		10, 11, 0,
		11, 12, 0,
		12, 13, 0,
		13, 14, 0,
		14, 15, 0,
		15, 16, 0,
		16, 17, 0,
		17, 18, 0,
		18, 19, 0,
		19, 20, 0,
		20, 21, 0,
		21, 22, 0,
		22, 23, 0,
		23, 24, 0,
		24, 25, 0,
		25, 26, 0,
		26, 27, 0,
		27, 28, 0,
		28, 29, 0,
		29, 30, 0,
		30, 31, 0,
		31, 32, 0,
		32, 33, 0,
		33, 34, 0,
		34, 35, 0,
		35, 36, 0,
		36, 37, 0,
		37, 38, 0,
		38, 39, 0,
		39, 40, 0,
		40, 1, 0
	};
	FillBuffer<UINT[]>(dv, devc, &ib, indices, sizeof(indices));

	if (!dd)
	{
		setView(&(MScaling(-w, h, 1) * (*world)), &mat_view, &mat_proj, diffuse, dv, devc, cb);
		devc->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		devc->DrawIndexed(120, 0, 0);
	}
	else
	{
		setView(&(MScaling(-w, h, 1) * (*world)), &mat_view, &mat_proj, diffuse, dv, devc, cb);
		devc->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		devc->DrawIndexed(120, 0, 0);
		setView(&(MScaling(w, h, 1) * (*world)), &mat_view, &mat_proj, diffuse, dv, devc, cb);
		devc->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		devc->DrawIndexed(120, 0, 0);
	}
}
void Draw3DBox(float w, float h, float b, color c, mat *world, color diffuse, ID3D11Device *dv, ID3D11DeviceContext *devc, ID3D11Buffer *vb, ID3D11Buffer *ib, ID3D11Buffer *cb)
{
	VERTEX_BASIC vertices[] =
	{
		{ -w, -h, -b, c },	//			7 o
		{ w, -h, -b, c },	//			  |		  6 o
		{ w, h, -b, c },	//	  3 o	  |			|
		{ -w, h, -b, c },	//		|	  |	2 o		|
		{ -w, -h, b, c },	//		|	4 o   |		|
		{ w, -h, b, c },	//		|		  |   5 o
		{ w, h, b, c },		//	  0 o		  |
		{ -w, h, b, c }		//				1 o
	};
	FillBuffer<VERTEX_BASIC[]>(dv, devc, &vb, vertices, sizeof(vertices));
	UINT indices[] =
	{
		0, 3, 2,
		0, 2, 1,
		0, 4, 7,
		0, 7, 3,
		0, 1, 5,
		0, 5, 4,
		6, 2, 3,
		6, 3, 7,
		6, 5, 1,
		6, 1, 2,
		6, 4, 5,
		4, 6, 7
	};
	FillBuffer<UINT[]>(dv, devc, &ib, indices, sizeof(indices));

	setView(world, &mat_view, &mat_proj, diffuse, dv, devc, cb);
	devc->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	devc->DrawIndexed(36, 0, 0);
}
void Draw3DBox(Vector3 l, color c, mat *world, color diffuse, ID3D11Device *dv, ID3D11DeviceContext *devc, ID3D11Buffer *vb, ID3D11Buffer *ib, ID3D11Buffer *cb)
{
	float w = l.x;
	float h = l.y;
	float b = l.z;

	VERTEX_BASIC vertices[] =
	{
		{ -w, -h, -b, c },	//			7 o
		{ w, -h, -b, c },	//			  |		  6 o
		{ w, h, -b, c },	//	  3 o	  |			|
		{ -w, h, -b, c },	//		|	  |	2 o		|
		{ -w, -h, b, c },	//		|	4 o   |		|
		{ w, -h, b, c },	//		|		  |   5 o
		{ w, h, b, c },		//	  0 o		  |
		{ -w, h, b, c }		//				1 o
	};
	FillBuffer<VERTEX_BASIC[]>(dv, devc, &vb, vertices, sizeof(vertices));
	UINT indices[] =
	{
		0, 3, 2,
		0, 2, 1,
		0, 4, 7,
		0, 7, 3,
		0, 1, 5,
		0, 5, 4,
		6, 2, 3,
		6, 3, 7,
		6, 5, 1,
		6, 1, 2,
		6, 4, 5,
		4, 6, 7
	};
	FillBuffer<UINT[]>(dv, devc, &ib, indices, sizeof(indices));

	setView(world, &mat_view, &mat_proj, diffuse, dv, devc, cb);
	devc->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	devc->DrawIndexed(36, 0, 0);
}
void Draw3DCube(float r, color c, mat *world, color diffuse, ID3D11Device *dv, ID3D11DeviceContext *devc, ID3D11Buffer *vb, ID3D11Buffer *ib, ID3D11Buffer *cb)
{
	Draw3DBox(r, r, r, c, world, diffuse, dv, devc, vb, ib, cb);
}
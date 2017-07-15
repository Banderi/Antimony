#include "Warnings.h"
#include "Geometry.h"

///

IDXGISwapChain *swapchain;
ID3D11Device *dev;
ID3D11DeviceContext *devcon;

ID3D11RenderTargetView *targettview;
ID3D11DepthStencilView *depthstencilview;

ID3D11RasterizerState *rasterizerstate;
ID3D11BlendState *blendstate;
ID3D11DepthStencilState *dss_enabled, *dss_disabled;

ID3D11Buffer *indexbuffer, *constantbuffer;

SHADER sh_main, sh_debug, sh_plain;
SHADER *sh_current = nullptr;

mat mat_identity, mat_temp, mat_temp2, mat_world, mat_view, mat_proj, mat_orthoview, mat_orthoproj;
float2  v2_origin = float2(0, 0);
float3  v3_origin = float3(0, 0, 0);

D3D11_INPUT_ELEMENT_DESC ied_basic[] = {
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};
D3D11_INPUT_ELEMENT_DESC ied_basic_animated[] = {
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "BLENDINDICES", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};
D3D11_INPUT_ELEMENT_DESC ied_main[] = {
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};
D3D11_INPUT_ELEMENT_DESC ied_main_skinned[] = {
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "BLENDINDICES", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};
//D3D11_INPUT_ELEMENT_DESC ied_VS_OUTPUT[] = {
//	{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
//};


//UINT vertex_stride = sizeof(VERTEX_BASIC);
//UINT vertex_offset = 0;

///

UINT* VertexCompound::dumpIndices()
{
	if (arrIndices == nullptr)
	{
		UINT size = index.size();
		arrIndices = new UINT[size];

		for (unsigned int i = 0; i < size; i++)
		{
			arrIndices[i] = index.at(i);
		}
	}
	return arrIndices;
}
VERTEX_BASIC* VertexCompound::dumpBASIC()
{
	if (arrBASIC == nullptr)
	{
		UINT size = position.size();
		arrBASIC = new VERTEX_BASIC[size];

		for (unsigned int i = 0; i < size; i++)
		{
			arrBASIC[i].x = position.at(i).x;
			arrBASIC[i].y = position.at(i).y;
			arrBASIC[i].z = position.at(i).z;
		}

		if (col.size() > 0)
		{
			for (unsigned int i = 0; i < size; i++)
			{
				arrBASIC[i].color = col.at(i + 1);
			}
		}
		else
		{
			for (unsigned int i = 0; i < size; i++)
			{
				arrBASIC[i].color = COLOR_WHITE;
			}
		}
	}
	return arrBASIC;
}
VERTEX_BASIC_SKINNED* VertexCompound::dumpBASIC_SKINNED()
{
	if (arrBASIC_SKINNED == nullptr)
	{
		UINT size = position.size();
	}
	return arrBASIC_SKINNED;
}
VERTEX_MAIN* VertexCompound::dumpMAIN()
{
	if (arrMAIN == nullptr)
	{
		UINT size = position.size();
	}
	return arrMAIN;
}
VERTEX_MAIN_SKINNED* VertexCompound::dumpMAIN_SKINNED()
{
	if (arrMAIN_SKINNED == nullptr)
	{
		UINT size = position.size();
	}
	return arrMAIN_SKINNED;
}
void VertexCompound::destroyDumps()
{
	if (arrIndices != nullptr)
	{
		delete[] arrIndices;
		arrIndices = nullptr;
	}
	if (arrBASIC != nullptr)
	{
		delete[] arrBASIC;
		arrBASIC = nullptr;
	}
	if (arrBASIC_SKINNED != nullptr)
	{
		delete[] arrBASIC_SKINNED;
		arrBASIC_SKINNED = nullptr;
	}
	if (arrMAIN != nullptr)
	{
		delete[] arrMAIN;
		arrMAIN = nullptr;
	}
	if (arrMAIN_SKINNED != nullptr)
	{
		delete[] arrMAIN_SKINNED;
		arrMAIN_SKINNED = nullptr;
	}
}

///

unsigned long RGBA2DWORD(int iR, int iG, int iB, int iA)
{
	return ((iA * 256 + iB) * 256 + iG) * 256 + iR;
}

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

float3 WorldToScreen(float3 p, mat *viewproj, float2 screen)
{
	float3 temp;
	temp = XMVector3Transform(p, *viewproj);

	float3 sp;
	sp.x = screen.x * (temp.x) / 2;
	sp.y = screen.y * (1 - ((temp.y + 2) / 2));
	sp /= temp.z;
	sp.z = temp.z;

	return sp;
}
float3 WorldToScreen(float3 p, mat *view, mat *proj, float2 screen)
{
	return WorldToScreen(p, &(*view * *proj), screen);
}
mat HingeBillboard(float3 p1, float3 p2, float3 pos)
{
	float m[16] = { 0 };
	float3 a, b, c;

	a = p1;				// x - forward / where the beam is going
	a.Normalize();

	b = p2;				// y - up / normal to camera
	b.Normalize();

	c = a.Cross(b);		// z - right / to fix the normal
	c.Normalize();

	b = c.Cross(a);		// y / adjusted
	b.Normalize();

	m[0] = a.x;
	m[1] = a.y;
	m[2] = a.z;
	m[5] = c.x;
	m[6] = c.y;
	m[7] = c.z;
	m[9] = b.x;
	m[10] = b.y;
	m[11] = b.z;
	m[13] = pos.x;
	m[14] = pos.y;
	m[15] = pos.z;

	return (mat)m;
}
mat FreeBillboard(float3 cam, float3 pos, float3 up, mat *view)
{
	float m[16] = { 0 };
	float3 a, b, c;
	mat invView;

	invView = XMMatrixInverse(NULL, *view);
	XMVector3TransformNormal(up, invView);

	a = (cam - pos);		// x - forward / the direction the billboard is looking at
	a.Normalize();

	b = cam.Cross(a);		// z - right / right vector of the billboard

	c = a.Cross(b);			// y - up / up vector of the billboard

	m[0] = b.x;
	m[1] = b.y;
	m[2] = b.z;
	m[5] = c.x;
	m[6] = c.y;
	m[7] = c.z;
	m[9] = a.x;
	m[10] = a.y;
	m[11] = a.z;
	m[13] = pos.x;
	m[14] = pos.y;
	m[15] = pos.z;

	return (mat)m;
}

HRESULT FillBuffer(ID3D11Device *dev, ID3D11DeviceContext *devcon, ID3D11Buffer **out, void *in, UINT size)
{
	HRESULT hr;

	D3D11_MAPPED_SUBRESOURCE ms;
	hr = devcon->Map(*out, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
	if (!handleErr(&hr, HRH_BUFFER_MAPPING, hr))
		return hr;
	D3D11_BUFFER_DESC bd;
	(*out)->GetDesc(&bd);
	UINT buffsize = bd.ByteWidth;
	if (size > buffsize)
	{
		handleErr(&hr, HRH_BUFFER_OVERFLOW, DISP_E_BUFFERTOOSMALL);
		return DISP_E_BUFFERTOOSMALL;
	}
	memcpy(ms.pData, (UINT*)in, size);
	devcon->Unmap(*out, NULL);

	return S_OK;
}

bool compileShader(HRESULT *hr, std::wstring shader, SHADER *sh, D3D11_INPUT_ELEMENT_DESC* ied, UINT stride, UINT size)
{
	ID3D10Blob *blob = nullptr;

	std::wstring str = L".\\Shaders\\" + shader + L".hlsl";

	LPCWSTR file = str.c_str();

	if (!handleErr(hr, HRH_SHADER_COMPILE, D3DCompileFromFile(file, 0, 0, "VShader", "vs_4_0", 0, 0, &blob, 0)))
		return 0;
	if (!handleErr(hr, HRH_SHADER_CREATE, dev->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), NULL, &sh->vs)))
		return 0;
	if (!handleErr(hr, HRH_SHADER_INPUTLAYOUT, dev->CreateInputLayout(ied_basic, 2, blob->GetBufferPointer(), blob->GetBufferSize(), &sh->il)))
		return 0;
	if (!handleErr(hr, HRH_SHADER_COMPILE, D3DCompileFromFile(file, 0, 0, "PShader", "ps_4_0", 0, 0, &blob, 0)))
		return 0;
	if (!handleErr(hr, HRH_SHADER_CREATE, dev->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), NULL, &sh->ps)))
		return 0;

	D3D11_BUFFER_DESC bd; // buffer description
	ZeroMemory(&bd, sizeof(bd));

	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = stride * size;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	if (!handleErr(hr, HRH_SHADER_VERTEXBUFFER, dev->CreateBuffer(&bd, NULL, &sh->vb)))
		return 0;

	sh->vbstride = stride;
	sh->vbsize = size;

	sh->ready = true;

	return 1; // all ok
}
bool setShader(SHADER *sh, ID3D11DeviceContext *devc)
{
	if (sh->ready)
	{
		if (sh_current == sh)
			return 1; // already set
		sh_current = sh;

		devc->IASetVertexBuffers(0, 1, &sh_current->vb, &sh_current->vbstride, &sh_current->vboffset);
		devc->VSSetShader(sh_current->vs, 0, 0);
		devc->PSSetShader(sh_current->ps, 0, 0);
		devc->IASetInputLayout(sh_current->il);
		devc->GSSetShader(sh_current->gs, 0, 0);
		devc->HSSetShader(sh_current->hs, 0, 0);
		devc->CSSetShader(sh_current->cs, 0, 0);
		devc->DSSetShader(sh_current->ds, 0, 0);
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

void Draw2DDot(float2 p, float t, color c, color diffuse, ID3D11Device *dv, ID3D11DeviceContext *devc, SHADER* sh, ID3D11Buffer *ib, ID3D11Buffer *cb)
{
	float half_t = t * 0.5;
	return Draw2DRectangle(t, t, p.x - half_t, p.y + half_t, c, diffuse, dv, devc, sh, ib, cb);
}
void Draw2DLineThin(float2 p1, float2 p2, color c1, color c2, color diffuse, ID3D11Device *dv, ID3D11DeviceContext *devc, SHADER* sh, ID3D11Buffer *ib, ID3D11Buffer *cb)
{
	p1.y = -p1.y;
	p2.y = -p2.y;
	VERTEX_BASIC vertices[] =
	{
		{ p1.x, p1.y, 0, c1 },
		{ p2.x, p2.y, 0, c2 }
	};
	//FillBuffer<VERTEX_BASIC[]>(dv, devc, &sh->vb, vertices, sizeof(vertices));
	FillBuffer(dv, devc, &sh->vb, &vertices, sizeof(vertices));

	setView(&mat_identity, &mat_orthoview, &mat_orthoproj, diffuse, dv, devc, cb);
	devc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	devc->Draw(2, 0);
}
void Draw2DLineThick(float2 p1, float2 p2, float t, color c1, color c2, color diffuse, ID3D11Device *dv, ID3D11DeviceContext *devc, SHADER* sh, ID3D11Buffer *ib, ID3D11Buffer *cb)
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
	FillBuffer(dv, devc, &sh->vb, &vertices, sizeof(vertices));
	UINT indices[] =
	{
		0, 3, 2,
		0, 1, 3
	};
	//FillBuffer<UINT[]>(dv, devc, &ib, indices, sizeof(indices));
	FillBuffer(dv, devc, &ib, &indices, sizeof(indices));

	setView(&(MRotZ(atan2(p2.y - p1.y, p2.x - p1.x)) * MTranslation(p1.x, p1.y, 0)), &mat_orthoview, &mat_orthoproj, diffuse, dv, devc, cb);
	devc->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	devc->DrawIndexed(6, 0, 0);
}
void Draw2DRectangle(float w, float h, float x, float y, color c, color diffuse, ID3D11Device *dv, ID3D11DeviceContext *devc, SHADER* sh, ID3D11Buffer *ib, ID3D11Buffer *cb)
{
	y = -y;
	VERTEX_BASIC vertices[] =
	{
		{ 0, 0, 0, c },
		{ w, 0, 0, c },
		{ w, h, 0, c },
		{ 0, h, 0, c }
	};
	FillBuffer(dv, devc, &sh->vb, &vertices, sizeof(vertices));
	UINT indices[] =
	{
		0, 2, 1,
		0, 3, 2
	};
	FillBuffer(dv, devc, &ib, &indices, sizeof(indices));

	setView(&MTranslation(x, y, 0), &mat_orthoview, &mat_orthoproj, diffuse, dv, devc, cb);
	devc->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	devc->DrawIndexed(6, 0, 0);
}
void Draw2DRectBorderThick(float w, float h, float x, float y, float t, color c, color diffuse, ID3D11Device *dv, ID3D11DeviceContext *devc, SHADER* sh, ID3D11Buffer *ib, ID3D11Buffer *cb)
{
	h = -h;
	Draw2DLineThick(float2(x - 0.5 * t, y), float2(x + w + 0.5 * t, y), t, c, c, diffuse, dv, devc, sh, ib, cb);
	Draw2DLineThick(float2(x + w, y + 0.5 * t), float2(x + w, y + h - 0.5 * t), t, c, c, diffuse, dv, devc, sh, ib, cb);
	Draw2DLineThick(float2(x + w + 0.5 * t, y + h), float2(x - 0.5 * t, y + h), t, c, c, diffuse, dv, devc, sh, ib, cb);
	Draw2DLineThick(float2(x, y + h - 0.5 * t), float2(x, y + 0.5 * t), t, c, c, diffuse, dv, devc, sh, ib, cb);
}
void Draw2DFullRect(float w, float h, float x, float y, float t, color c1, color c2, color diffuse, ID3D11Device *dv, ID3D11DeviceContext *devc, SHADER* sh, ID3D11Buffer *ib, ID3D11Buffer *cb)
{
	Draw2DRectangle(w, h, x, y, c1, diffuse, dv, devc, sh, ib, cb);
	Draw2DRectBorderThick(w, h, x, y, t, c2, diffuse, dv, devc, sh, ib, cb);
}
void Draw2DEllipses(float w, float h, float x, float y, color c, color diffuse, ID3D11Device *dv, ID3D11DeviceContext *devc, SHADER* sh, ID3D11Buffer *ib, ID3D11Buffer *cb)
{
	y = -y;
	VERTEX_BASIC vertices[] =
	{
		{ 0, 0, 0, c },
		{ cosf(0 * MATH_PI / 20), sinf(0 * MATH_PI / 20), 0, c },
		{ cosf(1 * MATH_PI / 20), sinf(1 * MATH_PI / 20), 0, c },
		{ cosf(2 * MATH_PI / 20), sinf(2 * MATH_PI / 20), 0, c },
		{ cosf(3 * MATH_PI / 20), sinf(3 * MATH_PI / 20), 0, c },
		{ cosf(4 * MATH_PI / 20), sinf(4 * MATH_PI / 20), 0, c },
		{ cosf(5 * MATH_PI / 20), sinf(5 * MATH_PI / 20), 0, c },
		{ cosf(6 * MATH_PI / 20), sinf(6 * MATH_PI / 20), 0, c },
		{ cosf(7 * MATH_PI / 20), sinf(7 * MATH_PI / 20), 0, c },
		{ cosf(8 * MATH_PI / 20), sinf(8 * MATH_PI / 20), 0, c },
		{ cosf(9 * MATH_PI / 20), sinf(9 * MATH_PI / 20), 0, c },
		{ cosf(10 * MATH_PI / 20), sinf(10 * MATH_PI / 20), 0, c },
		{ cosf(11 * MATH_PI / 20), sinf(11 * MATH_PI / 20), 0, c },
		{ cosf(12 * MATH_PI / 20), sinf(12 * MATH_PI / 20), 0, c },
		{ cosf(13 * MATH_PI / 20), sinf(13 * MATH_PI / 20), 0, c },
		{ cosf(14 * MATH_PI / 20), sinf(14 * MATH_PI / 20), 0, c },
		{ cosf(15 * MATH_PI / 20), sinf(15 * MATH_PI / 20), 0, c },
		{ cosf(16 * MATH_PI / 20), sinf(16 * MATH_PI / 20), 0, c },
		{ cosf(17 * MATH_PI / 20), sinf(17 * MATH_PI / 20), 0, c },
		{ cosf(18 * MATH_PI / 20), sinf(18 * MATH_PI / 20), 0, c },
		{ cosf(19 * MATH_PI / 20), sinf(19 * MATH_PI / 20), 0, c },
		{ cosf(20 * MATH_PI / 20), sinf(20 * MATH_PI / 20), 0, c },
		{ cosf(21 * MATH_PI / 20), sinf(21 * MATH_PI / 20), 0, c },
		{ cosf(22 * MATH_PI / 20), sinf(22 * MATH_PI / 20), 0, c },
		{ cosf(23 * MATH_PI / 20), sinf(23 * MATH_PI / 20), 0, c },
		{ cosf(24 * MATH_PI / 20), sinf(24 * MATH_PI / 20), 0, c },
		{ cosf(25 * MATH_PI / 20), sinf(25 * MATH_PI / 20), 0, c },
		{ cosf(26 * MATH_PI / 20), sinf(26 * MATH_PI / 20), 0, c },
		{ cosf(27 * MATH_PI / 20), sinf(27 * MATH_PI / 20), 0, c },
		{ cosf(28 * MATH_PI / 20), sinf(28 * MATH_PI / 20), 0, c },
		{ cosf(29 * MATH_PI / 20), sinf(29 * MATH_PI / 20), 0, c },
		{ cosf(30 * MATH_PI / 20), sinf(30 * MATH_PI / 20), 0, c },
		{ cosf(31 * MATH_PI / 20), sinf(31 * MATH_PI / 20), 0, c },
		{ cosf(32 * MATH_PI / 20), sinf(32 * MATH_PI / 20), 0, c },
		{ cosf(33 * MATH_PI / 20), sinf(33 * MATH_PI / 20), 0, c },
		{ cosf(34 * MATH_PI / 20), sinf(34 * MATH_PI / 20), 0, c },
		{ cosf(35 * MATH_PI / 20), sinf(35 * MATH_PI / 20), 0, c },
		{ cosf(36 * MATH_PI / 20), sinf(36 * MATH_PI / 20), 0, c },
		{ cosf(37 * MATH_PI / 20), sinf(37 * MATH_PI / 20), 0, c },
		{ cosf(38 * MATH_PI / 20), sinf(38 * MATH_PI / 20), 0, c },
		{ cosf(39 * MATH_PI / 20), sinf(39 * MATH_PI / 20), 0, c },
		{ cosf(40 * MATH_PI / 20), sinf(40 * MATH_PI / 20), 0, c }
	};
	FillBuffer(dv, devc, &sh->vb, &vertices, sizeof(vertices));
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
	FillBuffer(dv, devc, &ib, &indices, sizeof(indices));

	setView(&(MScaling(-w, h, 1) * MTranslation(x, y, 0)), &mat_orthoview, &mat_orthoproj, diffuse, dv, devc, cb);
	devc->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	devc->DrawIndexed(120, 0, 0);
}

void Draw3DLineThin(float3 p1, float3 p2, color c1, color c2, mat *world, color diffuse, ID3D11Device *dv, ID3D11DeviceContext *devc, SHADER* sh, ID3D11Buffer *ib, ID3D11Buffer *cb)
{
	VERTEX_BASIC vertices[] =
	{
		{ p1.x, p1.y, p1.z, c1 },
		{ p2.x, p2.y, p2.z, c2 }
	};
	FillBuffer(dv, devc, &sh->vb, &vertices, sizeof(vertices));

	setView(world, &mat_view, &mat_proj, diffuse, dv, devc, cb);
	devc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	devc->Draw(2, 0);
}
void Draw3DLineThick()
{
	// TODO: Implement bill-boarding
}
void Draw3DTriangle(float3 p1, float3 p2, float3 p3, color c, bool dd, mat *world, color diffuse, ID3D11Device *dv, ID3D11DeviceContext *devc, SHADER* sh, ID3D11Buffer *ib, ID3D11Buffer *cb)
{
	VERTEX_BASIC vertices[] =
	{
		{ p1.x, p1.y, p1.z, c },
		{ p2.x, p2.y, p2.z, c },
		{ p3.x, p3.y, p3.z, c }
	};
	FillBuffer(dv, devc, &sh->vb, &vertices, sizeof(vertices));
	UINT indices[] =
	{
		0, 2, 1,
		0, 1, 2
	};
	FillBuffer(dv, devc, &ib, &indices, sizeof(indices));

	setView(world, &mat_view, &mat_proj, diffuse, dv, devc, cb);
	devc->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	if (!dd)
		devc->DrawIndexed(3, 0, 0);
	else
		devc->DrawIndexed(6, 0, 0);
}
void Draw3DRectangle(float w, float h, color c, bool dd, mat *world, color diffuse, ID3D11Device *dv, ID3D11DeviceContext *devc, SHADER* sh, ID3D11Buffer *ib, ID3D11Buffer *cb)
{
	VERTEX_BASIC vertices[] =
	{
		{ 0, 0, 0, c },
		{ w, 0, 0, c },
		{ w, h, 0, c },
		{ 0, h, 0, c }
	};
	FillBuffer(dv, devc, &sh->vb, &vertices, sizeof(vertices));
	UINT indices[] =
	{
		0, 2, 1,
		0, 3, 2,
		0, 1, 2,
		0, 2, 3
	};
	FillBuffer(dv, devc, &ib, &indices, sizeof(indices));

	setView(world, &mat_view, &mat_proj, diffuse, dv, devc, cb);
	devc->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	if (!dd)
		devc->DrawIndexed(6, 0, 0);
	else
		devc->DrawIndexed(12, 0, 0);
}
void Draw3DEllipses(float w, float h, color c, bool dd, mat *world, color diffuse, ID3D11Device *dv, ID3D11DeviceContext *devc, SHADER* sh, ID3D11Buffer *ib, ID3D11Buffer *cb)
{
	VERTEX_BASIC vertices[] =
	{
		{ 0, 0, 0, c },
		{ cosf(0 * MATH_PI / 20), sinf(0 * MATH_PI / 20), 0, c },
		{ cosf(1 * MATH_PI / 20), sinf(1 * MATH_PI / 20), 0, c },
		{ cosf(2 * MATH_PI / 20), sinf(2 * MATH_PI / 20), 0, c },
		{ cosf(3 * MATH_PI / 20), sinf(3 * MATH_PI / 20), 0, c },
		{ cosf(4 * MATH_PI / 20), sinf(4 * MATH_PI / 20), 0, c },
		{ cosf(5 * MATH_PI / 20), sinf(5 * MATH_PI / 20), 0, c },
		{ cosf(6 * MATH_PI / 20), sinf(6 * MATH_PI / 20), 0, c },
		{ cosf(7 * MATH_PI / 20), sinf(7 * MATH_PI / 20), 0, c },
		{ cosf(8 * MATH_PI / 20), sinf(8 * MATH_PI / 20), 0, c },
		{ cosf(9 * MATH_PI / 20), sinf(9 * MATH_PI / 20), 0, c },
		{ cosf(10 * MATH_PI / 20), sinf(10 * MATH_PI / 20), 0, c },
		{ cosf(11 * MATH_PI / 20), sinf(11 * MATH_PI / 20), 0, c },
		{ cosf(12 * MATH_PI / 20), sinf(12 * MATH_PI / 20), 0, c },
		{ cosf(13 * MATH_PI / 20), sinf(13 * MATH_PI / 20), 0, c },
		{ cosf(14 * MATH_PI / 20), sinf(14 * MATH_PI / 20), 0, c },
		{ cosf(15 * MATH_PI / 20), sinf(15 * MATH_PI / 20), 0, c },
		{ cosf(16 * MATH_PI / 20), sinf(16 * MATH_PI / 20), 0, c },
		{ cosf(17 * MATH_PI / 20), sinf(17 * MATH_PI / 20), 0, c },
		{ cosf(18 * MATH_PI / 20), sinf(18 * MATH_PI / 20), 0, c },
		{ cosf(19 * MATH_PI / 20), sinf(19 * MATH_PI / 20), 0, c },
		{ cosf(20 * MATH_PI / 20), sinf(20 * MATH_PI / 20), 0, c },
		{ cosf(21 * MATH_PI / 20), sinf(21 * MATH_PI / 20), 0, c },
		{ cosf(22 * MATH_PI / 20), sinf(22 * MATH_PI / 20), 0, c },
		{ cosf(23 * MATH_PI / 20), sinf(23 * MATH_PI / 20), 0, c },
		{ cosf(24 * MATH_PI / 20), sinf(24 * MATH_PI / 20), 0, c },
		{ cosf(25 * MATH_PI / 20), sinf(25 * MATH_PI / 20), 0, c },
		{ cosf(26 * MATH_PI / 20), sinf(26 * MATH_PI / 20), 0, c },
		{ cosf(27 * MATH_PI / 20), sinf(27 * MATH_PI / 20), 0, c },
		{ cosf(28 * MATH_PI / 20), sinf(28 * MATH_PI / 20), 0, c },
		{ cosf(29 * MATH_PI / 20), sinf(29 * MATH_PI / 20), 0, c },
		{ cosf(30 * MATH_PI / 20), sinf(30 * MATH_PI / 20), 0, c },
		{ cosf(31 * MATH_PI / 20), sinf(31 * MATH_PI / 20), 0, c },
		{ cosf(32 * MATH_PI / 20), sinf(32 * MATH_PI / 20), 0, c },
		{ cosf(33 * MATH_PI / 20), sinf(33 * MATH_PI / 20), 0, c },
		{ cosf(34 * MATH_PI / 20), sinf(34 * MATH_PI / 20), 0, c },
		{ cosf(35 * MATH_PI / 20), sinf(35 * MATH_PI / 20), 0, c },
		{ cosf(36 * MATH_PI / 20), sinf(36 * MATH_PI / 20), 0, c },
		{ cosf(37 * MATH_PI / 20), sinf(37 * MATH_PI / 20), 0, c },
		{ cosf(38 * MATH_PI / 20), sinf(38 * MATH_PI / 20), 0, c },
		{ cosf(39 * MATH_PI / 20), sinf(39 * MATH_PI / 20), 0, c },
		{ cosf(40 * MATH_PI / 20), sinf(40 * MATH_PI / 20), 0, c }
	};
	FillBuffer(dv, devc, &sh->vb, &vertices, sizeof(vertices));
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
	FillBuffer(dv, devc, &ib, &indices, sizeof(indices));

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
void Draw3DBox(float w, float h, float b, color c, mat *world, color diffuse, ID3D11Device *dv, ID3D11DeviceContext *devc, SHADER* sh, ID3D11Buffer *ib, ID3D11Buffer *cb)
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
	FillBuffer(dv, devc, &sh->vb, &vertices, sizeof(vertices));
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
	FillBuffer(dv, devc, &ib, &indices, sizeof(indices));

	setView(world, &mat_view, &mat_proj, diffuse, dv, devc, cb);
	devc->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	devc->DrawIndexed(36, 0, 0);
}
void Draw3DBox(Vector3 l, color c, mat *world, color diffuse, ID3D11Device *dv, ID3D11DeviceContext *devc, SHADER* sh, ID3D11Buffer *ib, ID3D11Buffer *cb)
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
	FillBuffer(dv, devc, &sh->vb, &vertices, sizeof(vertices));
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
	FillBuffer(dv, devc, &ib, &indices, sizeof(indices));

	setView(world, &mat_view, &mat_proj, diffuse, dv, devc, cb);
	devc->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	devc->DrawIndexed(36, 0, 0);
}
void Draw3DCube(float r, color c, mat *world, color diffuse, ID3D11Device *dv, ID3D11DeviceContext *devc, SHADER* sh, ID3D11Buffer *ib, ID3D11Buffer *cb)
{
	Draw3DBox(r, r, r, c, world, diffuse, dv, devc, sh, ib, cb);
}
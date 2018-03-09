#include "Warnings.h"
#include "Geometry.h"
#include "Debug.h"
#include "FontRenderer.h"
#include "Gameplay.h"
#include "Path.h"

///

mat mat_identity;
float2 v2_origin = float2(0, 0);
float3 v3_origin = float3(0, 0, 0);

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
			arrBASIC[i].position = position.at(i);
			/*arrBASIC[i].x = position.at(i).x;
			arrBASIC[i].y = position.at(i).y;
			arrBASIC[i].z = position.at(i).z;*/
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
		arrMAIN = new VERTEX_MAIN[size];

		for (UINT i = 0; i < size; i++)
		{
			arrMAIN[i].position = position.at(i);
			arrMAIN[i].normal = normal.at(i);
			//arrMAIN[i].uv = uv.at(i);
		}
		if (uv.size() > 0)
		{
			for (UINT i = 0; i < size; i++)
			{
				arrMAIN[i].uv = uv.at(i);
			}
		}
		else
		{
			for (UINT i = 0; i < size; i++)
			{
				arrMAIN[i].uv = float2(0, 0);
			}
		}
		if (normalgroups.size() == 0)
			CalculateSmoothNormals(arrMAIN, size, &index[0], index.size(), &normalgroups);
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

unsigned long RGBA2DWORD(int iR, int iG, int indexbuffer, int iA)
{
	return ((iA * 256 + indexbuffer) * 256 + iG) * 256 + iR;
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

float3 WorldToScreen(float3 p)
{
	mat *viewproj = &(Antimony::mat_view * Antimony::mat_proj);
	float2 screen = float2(Antimony::display.width, Antimony::display.height);
	float3 temp;
	temp = XMVector3Transform(p, *viewproj);

	float3 sp;
	sp.x = screen.x * (temp.x) / 2;
	sp.y = screen.y * (1 - ((temp.y + 2) / 2));
	if (temp.z != 0)
		sp /= temp.z;
	else
		sp = { 0 };
	sp.z = temp.z;

	return sp;
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

void CalculateSmoothNormals(VERTEX_MAIN vin[], UINT vcount, UINT iin[], UINT icount, std::vector<std::vector<float3>> *normalgroups, bool surf_weighting, bool ang_weighting)
{
	for (UINT v = 0; v < vcount; v++)
	{
		vin[v].normal = v3_origin;
		std::vector<float3> ngroup;
		normalgroups->push_back(ngroup);
	}
	for (UINT f = 0; f < icount; f += 3)
	{
		float3 p1 = vin[iin[f]].position;
		float3 p2 = vin[iin[f + 1]].position;
		float3 p3 = vin[iin[f + 2]].position;

		/*float3 n1 = (p2 - p1).Cross(p3 - p1);
		float3 n2 = (p3 - p2).Cross(p1 - p2);
		float3 n3 = (p1 - p3).Cross(p2 - p3);*/

		float3 n = (p2 - p1).Cross(p3 - p1);

		if (!surf_weighting)
		{
			n.Normalize();
		}

		if (!ang_weighting)
		{
			normalgroups->at(iin[f]).push_back(n);
			normalgroups->at(iin[f + 1]).push_back(n);
			normalgroups->at(iin[f + 2]).push_back(n);
			continue;
		}

		float angle = XMVector3AngleBetweenVectors((p2 - p1), (p3 - p1)).vector_floats[0];
		float w = (angle);
		float3 na = n * w;
		normalgroups->at(iin[f]).push_back(na);

		angle = XMVector3AngleBetweenVectors((p3 - p2), (p1 - p2)).vector_floats[0];
		w = (angle);
		na = n * w;
		normalgroups->at(iin[f + 1]).push_back(na);

		angle = XMVector3AngleBetweenVectors((p1 - p3), (p2 - p3)).vector_floats[0];
		w = (angle);
		na = n * w;
		normalgroups->at(iin[f + 2]).push_back(na);

		////if (std::find(normalgroups.at(iin[f]).begin(), normalgroups.at(iin[f]).end(), n) == normalgroups.at(iin[f]).end())
		//	normalgroups->at(iin[f]).push_back(n1);
		////if (std::find(normalgroups.at(iin[f + 1]).begin(), normalgroups.at(iin[f + 1]).end(), n) == normalgroups.at(iin[f + 1]).end())
		//	normalgroups->at(iin[f + 1]).push_back(n2);
		////if (std::find(normalgroups.at(iin[f + 2]).begin(), normalgroups.at(iin[f + 2]).end(), n) == normalgroups.at(iin[f + 2]).end())
		//	normalgroups->at(iin[f + 2]).push_back(n3);

		//n.Normalize();

		/*float sin_alpha = n.Length() / ((p2 - p1).Length() * (p3 - p1).Length());
		n = n * rand();*/


		//// loop through all vertices in face A
		//for (UINT vt = 0; vt < 3; vt++)
		//{
		//	// for each face B in mesh
		//	for (UINT f2 = 0; f2 < icount; f2 += 3)
		//	{
		//		// ignore self
		//		if (f != f2)
		//		{
		//			float3 B1 = vin[iin[f2]].position;
		//			float3 B2 = vin[iin[f2 + 1]].position;
		//			float3 B3 = vin[iin[f2 + 2]].position;

		//			float3 n2 = (B2 - B1).Cross(B3 - B1);

		//			// accumulate normal
		//			// v1, v2, v3 are the vertices of face A
		//			// face B shares v1
		//			if ((vin[iin[f2]].position == p1) || (vin[iin[f2 + 1]].position == p1) || (vin[iin[f2 + 2]].position == p1))
		//			{
		//				auto angle = XMVector3AngleBetweenVectors((p1 - p2), (p1 - p3)).m128_f32[0];
		//				n += n2 * angle; // multiply by angle
		//			}
		//			// face B shares v2
		//			if ((vin[iin[f2]].position == p2) || (vin[iin[f2 + 1]].position == p2) || (vin[iin[f2 + 2]].position == p2))
		//			{
		//				auto angle = XMVector3AngleBetweenVectors((p2 - p1), (p2 - p3)).m128_f32[0];
		//				//angle = angle_between_vectors(v2 - v1 , v2 - v3)
		//				n += n2 * angle; // multiply by angle
		//			}
		//			// face B shares v3
		//			if ((vin[iin[f2]].position == p3) || (vin[iin[f2 + 1]].position == p3) || (vin[iin[f2 + 2]].position == p3))
		//			{
		//				auto angle = XMVector3AngleBetweenVectors((p3 - p1), (p3 - p2)).m128_f32[0];
		//				//angle = angle_between_vectors(v3 - v1 , v3 - v2)
		//				n += n2 * angle; // multiply by angle
		//			}
		//		}
		//	}
		//	n.Normalize();
		//}




		//n.Normalize();
		//n = (n - float3(XMVector3Normalize(n))) * 0.5;

		/*vin[iin[f]].normal += n;
		vin[iin[f + 1]].normal += n;
		vin[iin[f + 2]].normal += n;*/
	}
	for (UINT v = 0; v < vcount; v++)
	{
		float3 n = v3_origin;
		for (UINT nc = 0; nc < normalgroups->at(v).size(); nc++)
		{
			n += normalgroups->at(v).at(nc);
		}
		//n = n / normalgroups->at(v).size();

		n.Normalize();
		n = XMVector3Normalize(n);

		vin[v].normal = n;
	}
}

HRESULT Antimony::FillBuffer(ID3D11Device *dev, ID3D11DeviceContext *devcon, ID3D11Buffer **out, void *in, UINT size)
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
bool Antimony::compileShader(HRESULT *hr, ::std::wstring shader, Shader *sh, D3D11_INPUT_ELEMENT_DESC ied[], UINT elems, UINT stride, UINT size)
{
	ID3D10Blob *blob = nullptr;

	std::wstring fullpath = filePath(L"\\Shaders\\", shader + L".hlsl");
	LPCWSTR file = fullpath.c_str();

	if (!handleErr(hr, HRH_SHADER_COMPILE, D3DCompileFromFile(file, 0, 0, "VShader", "vs_4_0", 0, 0, &blob, 0)))
		return 0;
	if (!handleErr(hr, HRH_SHADER_CREATE, dev->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), NULL, &sh->vs)))
		return 0;
	if (!handleErr(hr, HRH_SHADER_INPUTLAYOUT, dev->CreateInputLayout(ied, elems, blob->GetBufferPointer(), blob->GetBufferSize(), &sh->il)))
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

	sh->ied = ied;

	sh->ready = true;

	return 1; // all ok
}
bool Antimony::setShader(Shader *sh, ID3D11DeviceContext *devcon)
{
	if (sh->ready)
	{
		if (sh_current == sh)
			return 1; // already set
		sh_current = sh;

		devcon->IASetVertexBuffers(0, 1, &sh->vb, &sh->vbstride, &sh->vboffset);
		devcon->VSSetShader(sh->vs, 0, 0);
		devcon->PSSetShader(sh->ps, 0, 0);
		devcon->IASetInputLayout(sh->il);
		devcon->GSSetShader(sh->gs, 0, 0);
		devcon->HSSetShader(sh->hs, 0, 0);
		devcon->CSSetShader(sh->cs, 0, 0);
		devcon->DSSetShader(sh->ds, 0, 0);
		return 1; // all ok
	}
	return 0; // NOPE
}
void Antimony::setDepthBufferState(bool state)
{
	if (state == true)
		devcon->OMSetDepthStencilState(dss_enabled, 1);
	else
		devcon->OMSetDepthStencilState(dss_disabled, 1);
}
HRESULT Antimony::setView(mat *m_world, mat *view, mat *proj, color diffuse, ID3D11Device *dev, ID3D11DeviceContext *devcon, ID3D11Buffer *constantbuffer)
{
	ConstantBuffer buffer;

	buffer.world = TransposeMatrix(*m_world);
	buffer.view = TransposeMatrix(*view);
	buffer.projection = TransposeMatrix(*proj);

	XMVECTOR sv, rv, tv;
	XMMatrixDecompose(&sv, &rv, &tv, *m_world);
	mat r = XMMatrixRotationQuaternion(rv);

	buffer.normal = TransposeMatrix(r);
	buffer.diffuse = diffuse;
	buffer.camera = float4(camera_main.getPos());

	return FillBuffer(dev, devcon, &constantbuffer, &buffer, sizeof(buffer));
}

void Antimony::Draw2DDot(float2 p, float t, color c, color diffuse)
{
	float half_t = t * 0.5;
	return Draw2DRectangle(t, t, p.x - half_t, p.y + half_t, c, diffuse);
}
void Antimony::Draw2DLineThin(float2 p1, float2 p2, color c1, color c2, color diffuse)
{
	assert(sh_current->ied == ied_basic);

	p1.y = -p1.y;
	p2.y = -p2.y;
	VERTEX_BASIC vertices[] =
	{
		{ p1, c1 },
		{ p2, c2 }
	};
	/*VERTEX_BASIC vertices[] =
	{
		{ p1.x, p1.y, 0, c1 },
		{ p2.x, p2.y, 0, c2 }
	};*/
	//FillBuffer<VERTEX_BASIC[]>(dev, devcon, &sh_current->vb, vertices, sizeof(vertices));
	FillBuffer(dev, devcon, &sh_current->vb, &vertices, sizeof(vertices));

	setView(&mat_identity, &mat_orthoview, &mat_orthoproj, diffuse, dev, devcon, constantbuffer);
	devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	devcon->Draw(2, 0);
}
void Antimony::Draw2DLineThick(float2 p1, float2 p2, float t, color c1, color c2, color diffuse)
{
	assert(sh_current->ied == ied_basic);

	p1.y = -p1.y;
	p2.y = -p2.y;
	float dist = *XMVector2Length(p1 - p2).vector_floats;

	VERTEX_BASIC vertices[] =
	{
		{ float3(0, -0.5 * t, 0), c1 },
		{ float3(0, 0.5 * t, 0), c1 },
		{ float3(dist, -0.5 * t, 0), c2 },
		{ float3(dist, 0.5 * t, 0), c2 }
	};
	/*VERTEX_BASIC vertices[] =
	{
		{ 0, -0.5 * t, 0, c1 },
		{ 0, 0.5 * t, 0, c1 },
		{ dist, -0.5 * t, 0, c2 },
		{ dist, 0.5 * t, 0, c2 }
	};*/
	FillBuffer(dev, devcon, &sh_current->vb, &vertices, sizeof(vertices));
	UINT indices[] =
	{
		0, 3, 2,
		0, 1, 3
	};
	//FillBuffer<UINT[]>(dev, devcon, &indexbuffer, indices, sizeof(indices));
	FillBuffer(dev, devcon, &indexbuffer, &indices, sizeof(indices));

	setView(&(MRotZ(atan2(p2.y - p1.y, p2.x - p1.x)) * MTranslation(p1.x, p1.y, 0)), &mat_orthoview, &mat_orthoproj, diffuse, dev, devcon, constantbuffer);
	devcon->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	devcon->DrawIndexed(6, 0, 0);
}
void Antimony::Draw2DRectangle(float w, float h, float x, float y, color c, color diffuse)
{
	assert(sh_current->ied == ied_basic);

	y = -y;
	VERTEX_BASIC vertices[] =
	{
		{ float3(0, 0, 0), c },
		{ float3(w, 0, 0), c },
		{ float3(w, h, 0), c },
		{ float3(0, h, 0), c }
	};
	FillBuffer(dev, devcon, &sh_current->vb, &vertices, sizeof(vertices));
	UINT indices[] =
	{
		0, 2, 1,
		0, 3, 2
	};
	FillBuffer(dev, devcon, &indexbuffer, &indices, sizeof(indices));

	setView(&MTranslation(x, y, 0), &mat_orthoview, &mat_orthoproj, diffuse, dev, devcon, constantbuffer);
	devcon->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	devcon->DrawIndexed(6, 0, 0);
}
void Antimony::Draw2DRectBorderThick(float w, float h, float x, float y, float t, color c, color diffuse)
{
	assert(sh_current->ied == ied_basic);

	h = -h;
	Draw2DLineThick(float2(x - 0.5 * t, y), float2(x + w + 0.5 * t, y), t, c, c, diffuse);
	Draw2DLineThick(float2(x + w, y + 0.5 * t), float2(x + w, y + h - 0.5 * t), t, c, c, diffuse);
	Draw2DLineThick(float2(x + w + 0.5 * t, y + h), float2(x - 0.5 * t, y + h), t, c, c, diffuse);
	Draw2DLineThick(float2(x, y + h - 0.5 * t), float2(x, y + 0.5 * t), t, c, c, diffuse);
}
void Antimony::Draw2DFullRect(float w, float h, float x, float y, float t, color c1, color c2, color diffuse)
{
	Draw2DRectangle(w, h, x, y, c1, diffuse);
	Draw2DRectBorderThick(w, h, x, y, t, c2, diffuse);
}
void Antimony::Draw2DEllipses(float w, float h, float x, float y, color c, color diffuse)
{
	assert(sh_current->ied == ied_basic);

	y = -y;
	VERTEX_BASIC vertices[] =
	{
		{ float3(0, 0, 0), c },
		{ float3(cosf(0 * MATH_PI / 20), sinf(0 * MATH_PI / 20), 0), c },
		{ float3(cosf(1 * MATH_PI / 20), sinf(1 * MATH_PI / 20), 0), c },
		{ float3(cosf(2 * MATH_PI / 20), sinf(2 * MATH_PI / 20), 0), c },
		{ float3(cosf(3 * MATH_PI / 20), sinf(3 * MATH_PI / 20), 0), c },
		{ float3(cosf(4 * MATH_PI / 20), sinf(4 * MATH_PI / 20), 0), c },
		{ float3(cosf(5 * MATH_PI / 20), sinf(5 * MATH_PI / 20), 0), c },
		{ float3(cosf(6 * MATH_PI / 20), sinf(6 * MATH_PI / 20), 0), c },
		{ float3(cosf(7 * MATH_PI / 20), sinf(7 * MATH_PI / 20), 0), c },
		{ float3(cosf(8 * MATH_PI / 20), sinf(8 * MATH_PI / 20), 0), c },
		{ float3(cosf(9 * MATH_PI / 20), sinf(9 * MATH_PI / 20), 0), c },
		{ float3(cosf(10 * MATH_PI / 20), sinf(10 * MATH_PI / 20), 0), c },
		{ float3(cosf(11 * MATH_PI / 20), sinf(11 * MATH_PI / 20), 0), c },
		{ float3(cosf(12 * MATH_PI / 20), sinf(12 * MATH_PI / 20), 0), c },
		{ float3(cosf(13 * MATH_PI / 20), sinf(13 * MATH_PI / 20), 0), c },
		{ float3(cosf(14 * MATH_PI / 20), sinf(14 * MATH_PI / 20), 0), c },
		{ float3(cosf(15 * MATH_PI / 20), sinf(15 * MATH_PI / 20), 0), c },
		{ float3(cosf(16 * MATH_PI / 20), sinf(16 * MATH_PI / 20), 0), c },
		{ float3(cosf(17 * MATH_PI / 20), sinf(17 * MATH_PI / 20), 0), c },
		{ float3(cosf(18 * MATH_PI / 20), sinf(18 * MATH_PI / 20), 0), c },
		{ float3(cosf(19 * MATH_PI / 20), sinf(19 * MATH_PI / 20), 0), c },
		{ float3(cosf(20 * MATH_PI / 20), sinf(20 * MATH_PI / 20), 0), c },
		{ float3(cosf(21 * MATH_PI / 20), sinf(21 * MATH_PI / 20), 0), c },
		{ float3(cosf(22 * MATH_PI / 20), sinf(22 * MATH_PI / 20), 0), c },
		{ float3(cosf(23 * MATH_PI / 20), sinf(23 * MATH_PI / 20), 0), c },
		{ float3(cosf(24 * MATH_PI / 20), sinf(24 * MATH_PI / 20), 0), c },
		{ float3(cosf(25 * MATH_PI / 20), sinf(25 * MATH_PI / 20), 0), c },
		{ float3(cosf(26 * MATH_PI / 20), sinf(26 * MATH_PI / 20), 0), c },
		{ float3(cosf(27 * MATH_PI / 20), sinf(27 * MATH_PI / 20), 0), c },
		{ float3(cosf(28 * MATH_PI / 20), sinf(28 * MATH_PI / 20), 0), c },
		{ float3(cosf(29 * MATH_PI / 20), sinf(29 * MATH_PI / 20), 0), c },
		{ float3(cosf(30 * MATH_PI / 20), sinf(30 * MATH_PI / 20), 0), c },
		{ float3(cosf(31 * MATH_PI / 20), sinf(31 * MATH_PI / 20), 0), c },
		{ float3(cosf(32 * MATH_PI / 20), sinf(32 * MATH_PI / 20), 0), c },
		{ float3(cosf(33 * MATH_PI / 20), sinf(33 * MATH_PI / 20), 0), c },
		{ float3(cosf(34 * MATH_PI / 20), sinf(34 * MATH_PI / 20), 0), c },
		{ float3(cosf(35 * MATH_PI / 20), sinf(35 * MATH_PI / 20), 0), c },
		{ float3(cosf(36 * MATH_PI / 20), sinf(36 * MATH_PI / 20), 0), c },
		{ float3(cosf(37 * MATH_PI / 20), sinf(37 * MATH_PI / 20), 0), c },
		{ float3(cosf(38 * MATH_PI / 20), sinf(38 * MATH_PI / 20), 0), c },
		{ float3(cosf(39 * MATH_PI / 20), sinf(39 * MATH_PI / 20), 0), c },
		{ float3(cosf(40 * MATH_PI / 20), sinf(40 * MATH_PI / 20), 0), c }
	};
	/*VERTEX_BASIC vertices[] =
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
	};*/
	FillBuffer(dev, devcon, &sh_current->vb, &vertices, sizeof(vertices));
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
	FillBuffer(dev, devcon, &indexbuffer, &indices, sizeof(indices));

	setView(&(MScaling(-w, h, 1) * MTranslation(x, y, 0)), &mat_orthoview, &mat_orthoproj, diffuse, dev, devcon, constantbuffer);
	devcon->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	devcon->DrawIndexed(120, 0, 0);
}

void Antimony::Draw3DLineThin(float3 p1, float3 p2, color c1, color c2, mat *m_world, color diffuse)
{
	assert(sh_current->ied == ied_basic);

	VERTEX_BASIC vertices[] =
	{
		{ p1, c1 },
		{ p2, c2 }
	};
	FillBuffer(dev, devcon, &sh_current->vb, &vertices, sizeof(vertices));

	setView(m_world, &mat_view, &mat_proj, diffuse, dev, devcon, constantbuffer);
	devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	devcon->Draw(2, 0);
}
void Antimony::Draw3DLineThick()
{
	//assert(sh_current->ied == ied_main);

	// TODO: Implement bill-boarding
}
void Antimony::Draw3DTriangle(float3 p1, float3 p2, float3 p3, color c, bool dd, mat *m_world, bool ortho, color diffuse)
{
	assert(sh_current->ied == ied_main);

	VERTEX_MAIN vertices[] =
	{
		{ float3(p1.x, p1.y, p1.z), float3(0, 0, 0), float2(0, 0) },
		{ float3(p2.x, p2.y, p2.z), float3(0, 0, 0), float2(0, 1) },
		{ float3(p3.x, p3.y, p3.z), float3(0, 0, 0), float2(1, 1) }
	};
	FillBuffer(dev, devcon, &sh_current->vb, &vertices, sizeof(vertices));
	UINT indices[] =
	{
		0, 2, 1,
		0, 1, 2
	};
	FillBuffer(dev, devcon, &indexbuffer, &indices, sizeof(indices));

	if (ortho)
		setView(m_world, &mat_orthoview, &mat_orthoproj, c * diffuse, dev, devcon, constantbuffer);
	else
		setView(m_world, &mat_view, &mat_proj, c * diffuse, dev, devcon, constantbuffer);
	devcon->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	if (!dd)
		devcon->DrawIndexed(3, 0, 0);
	else
		devcon->DrawIndexed(6, 0, 0);
}
void Antimony::Draw3DRectangle(float w, float h, color c, bool dd, mat *m_world, bool ortho, color diffuse)
{
	assert(sh_current->ied == ied_main);

	VERTEX_MAIN vertices[] =
	{
		{ float3(-w * 0.5, 0, -h * 0.5), float3(0, 1, 0), float2(0, 0) },
		{ float3(w * 0.5, 0, -h * 0.5), float3(0, 1, 0), float2(0, 1) },
		{ float3(w * 0.5, 0, h * 0.5), float3(0, 1, 0), float2(1, 1) },
		{ float3(-w * 0.5, 0, h * 0.5), float3(0, 1, 0), float2(1, 0) }
	};
	FillBuffer(dev, devcon, &sh_current->vb, &vertices, sizeof(vertices));
	UINT indices[] =
	{
		0, 2, 1,
		0, 3, 2,
		0, 1, 2,
		0, 2, 3
	};
	FillBuffer(dev, devcon, &indexbuffer, &indices, sizeof(indices));

	if (ortho)
		setView(m_world, &mat_orthoview, &mat_orthoproj, c * diffuse, dev, devcon, constantbuffer);
	else
		setView(m_world, &mat_view, &mat_proj, c * diffuse, dev, devcon, constantbuffer);
	devcon->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	if (!dd)
		devcon->DrawIndexed(6, 0, 0);
	else
		devcon->DrawIndexed(12, 0, 0);

	//DrawNormals(vertices, sizeof(vertices) / sizeof(VERTEX_MAIN), nullptr);
}
void Antimony::Draw3DEllipses(float w, float h, color c, float sec, bool dd, mat *m_world, bool ortho, color diffuse)
{
	if (sec > 100)
		sec = 100;
	if (sec <= 0)
		return;

	float sec_rad = sec / 50.0f * MATH_PI;

	VERTEX_BASIC vertices[] =
	{
		{ float3(0, 0, 0), c },
		{ float3(cosf(0 * MATH_PI / 20), sinf(0 * MATH_PI / 20), 0), c },
		{ float3(cosf(1 * MATH_PI / 20), sinf(1 * MATH_PI / 20), 0), c },
		{ float3(cosf(2 * MATH_PI / 20), sinf(2 * MATH_PI / 20), 0), c },
		{ float3(cosf(3 * MATH_PI / 20), sinf(3 * MATH_PI / 20), 0), c },
		{ float3(cosf(4 * MATH_PI / 20), sinf(4 * MATH_PI / 20), 0), c },
		{ float3(cosf(5 * MATH_PI / 20), sinf(5 * MATH_PI / 20), 0), c },
		{ float3(cosf(6 * MATH_PI / 20), sinf(6 * MATH_PI / 20), 0), c },
		{ float3(cosf(7 * MATH_PI / 20), sinf(7 * MATH_PI / 20), 0), c },
		{ float3(cosf(8 * MATH_PI / 20), sinf(8 * MATH_PI / 20), 0), c },
		{ float3(cosf(9 * MATH_PI / 20), sinf(9 * MATH_PI / 20), 0), c },
		{ float3(cosf(10 * MATH_PI / 20), sinf(10 * MATH_PI / 20), 0), c },
		{ float3(cosf(11 * MATH_PI / 20), sinf(11 * MATH_PI / 20), 0), c },
		{ float3(cosf(12 * MATH_PI / 20), sinf(12 * MATH_PI / 20), 0), c },
		{ float3(cosf(13 * MATH_PI / 20), sinf(13 * MATH_PI / 20), 0), c },
		{ float3(cosf(14 * MATH_PI / 20), sinf(14 * MATH_PI / 20), 0), c },
		{ float3(cosf(15 * MATH_PI / 20), sinf(15 * MATH_PI / 20), 0), c },
		{ float3(cosf(16 * MATH_PI / 20), sinf(16 * MATH_PI / 20), 0), c },
		{ float3(cosf(17 * MATH_PI / 20), sinf(17 * MATH_PI / 20), 0), c },
		{ float3(cosf(18 * MATH_PI / 20), sinf(18 * MATH_PI / 20), 0), c },
		{ float3(cosf(19 * MATH_PI / 20), sinf(19 * MATH_PI / 20), 0), c },
		{ float3(cosf(20 * MATH_PI / 20), sinf(20 * MATH_PI / 20), 0), c },
		{ float3(cosf(21 * MATH_PI / 20), sinf(21 * MATH_PI / 20), 0), c },
		{ float3(cosf(22 * MATH_PI / 20), sinf(22 * MATH_PI / 20), 0), c },
		{ float3(cosf(23 * MATH_PI / 20), sinf(23 * MATH_PI / 20), 0), c },
		{ float3(cosf(24 * MATH_PI / 20), sinf(24 * MATH_PI / 20), 0), c },
		{ float3(cosf(25 * MATH_PI / 20), sinf(25 * MATH_PI / 20), 0), c },
		{ float3(cosf(26 * MATH_PI / 20), sinf(26 * MATH_PI / 20), 0), c },
		{ float3(cosf(27 * MATH_PI / 20), sinf(27 * MATH_PI / 20), 0), c },
		{ float3(cosf(28 * MATH_PI / 20), sinf(28 * MATH_PI / 20), 0), c },
		{ float3(cosf(29 * MATH_PI / 20), sinf(29 * MATH_PI / 20), 0), c },
		{ float3(cosf(30 * MATH_PI / 20), sinf(30 * MATH_PI / 20), 0), c },
		{ float3(cosf(31 * MATH_PI / 20), sinf(31 * MATH_PI / 20), 0), c },
		{ float3(cosf(32 * MATH_PI / 20), sinf(32 * MATH_PI / 20), 0), c },
		{ float3(cosf(33 * MATH_PI / 20), sinf(33 * MATH_PI / 20), 0), c },
		{ float3(cosf(34 * MATH_PI / 20), sinf(34 * MATH_PI / 20), 0), c },
		{ float3(cosf(35 * MATH_PI / 20), sinf(35 * MATH_PI / 20), 0), c },
		{ float3(cosf(36 * MATH_PI / 20), sinf(36 * MATH_PI / 20), 0), c },
		{ float3(cosf(37 * MATH_PI / 20), sinf(37 * MATH_PI / 20), 0), c },
		{ float3(cosf(38 * MATH_PI / 20), sinf(38 * MATH_PI / 20), 0), c },
		{ float3(cosf(39 * MATH_PI / 20), sinf(39 * MATH_PI / 20), 0), c },
		{ float3(cosf(sec_rad), sinf(sec_rad), 0), c }
	};
	FillBuffer(dev, devcon, &sh_current->vb, &vertices, sizeof(vertices));

	UINT n = floor(sec * 40 / 100.1f + 1);

	UINT indices[] =
	{
		n, 41, 0,
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
		39, 40, 0
		//40, 1, 0
	};
	FillBuffer(dev, devcon, &indexbuffer, &indices, sizeof(indices));

	if (!dd)
	{
		if (ortho)
			setView(&(MScaling(-w, h, 1) * (*m_world)), &mat_orthoview, &mat_orthoproj, diffuse, dev, devcon, constantbuffer);
		else
			setView(&(MScaling(-w, h, 1) * (*m_world)), &mat_view, &mat_proj, diffuse, dev, devcon, constantbuffer);
		devcon->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		devcon->DrawIndexed(3 * n, 0, 0);
	}
	else
	{
		if (ortho)
			setView(&(MScaling(-w, h, 1) * (*m_world)), &mat_orthoview, &mat_orthoproj, diffuse, dev, devcon, constantbuffer);
		else
			setView(&(MScaling(-w, h, 1) * (*m_world)), &mat_view, &mat_proj, diffuse, dev, devcon, constantbuffer);
		devcon->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		devcon->DrawIndexed(3 * n, 0, 0);
		if (ortho)
			setView(&(MScaling(-w, h, 1) * (*m_world)), &mat_orthoview, &mat_orthoproj, diffuse, dev, devcon, constantbuffer);
		else
			setView(&(MScaling(-w, h, 1) * (*m_world)), &mat_view, &mat_proj, diffuse, dev, devcon, constantbuffer);
		devcon->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		devcon->DrawIndexed(3 * n, 0, 0);
	}
}
void Antimony::DrawCylinderSide(float r, float h, color c, float sec, bool dd, mat *m_world, bool ortho, color diffuse)
{

}
void Antimony::DrawCylinder(float r, float h, color c, float sec, bool dd, mat *m_world, bool ortho, color diffuse)
{

}

void Antimony::Draw3DBox(float3 l, color c, mat *m_world, bool ortho, color diffuse)
{
	assert(sh_current->ied == ied_main);

	float w = l.x;
	float h = l.y;
	float b = l.z;

	VERTEX_MAIN vertices[] =
	{
		{ float3(-w, -h, -b), XMVector3Normalize(float3(-1, -1, -1)), float2(0, 0) },	//			7 o
		{ float3(w, -h, -b), XMVector3Normalize(float3(1, -1, -1)), float2(0, 0) },		//			  |		  6 o
		{ float3(w, h, -b), XMVector3Normalize(float3(1, 1, -1)), float2(0, 0) },		//	  3 o	  |			|
		{ float3(-w, h, -b), XMVector3Normalize(float3(-1, 1, -1)), float2(0, 0) },		//		|	  |	2 o		|
		{ float3(-w, -h, b), XMVector3Normalize(float3(-1, -1, 1)), float2(0, 0) },		//		|	4 o   |		|
		{ float3(w, -h, b), XMVector3Normalize(float3(1, -1, 1)), float2(0, 0) },		//		|		  |   5 o
		{ float3(w, h, b), XMVector3Normalize(float3(1, 1, 1)), float2(0, 0) },			//	  0 o		  |
		{ float3(-w, h, b), XMVector3Normalize(float3(-1, 1, 1)), float2(0, 0) }		//				1 o
	};
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
	FillBuffer(dev, devcon, &indexbuffer, &indices, sizeof(indices));

	//CalculateSmoothNormals(vertices, sizeof(vertices) / sizeof(VERTEX_MAIN), indices, sizeof(indices) / sizeof(UINT));

	FillBuffer(dev, devcon, &sh_current->vb, &vertices, sizeof(vertices));

	diffuse = c * diffuse;
	setView(m_world, &mat_view, &mat_proj, diffuse, dev, devcon, constantbuffer);
	devcon->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	devcon->DrawIndexed(36, 0, 0);

	//DrawNormals(vertices, sizeof(vertices) / sizeof(VERTEX_MAIN), nullptr);
}
void Antimony::Draw3DBox(float w, float h, float b, color c, mat *m_world, bool ortho, color diffuse)
{
	Draw3DBox(float3(w, h, b), c, m_world, ortho, diffuse);
}
void Antimony::Draw3DCube(float r, color c, mat *m_world, bool ortho, color diffuse)
{
	Draw3DBox(r, r, r, c, m_world, ortho, diffuse);
}

void Antimony::DrawMeshPlain(VertexCompound *mesh, mat *m_world, bool ortho, color diffuse)
{
	auto sh_temp = sh_current;
	setShader(SHADERS_PLAIN);
	assert(sh_current->ied == ied_basic);

	UINT vcount = mesh->position.size();
	UINT icount = mesh->index.size();

	VERTEX_BASIC *vertices = mesh->dumpBASIC();
	UINT *indices = mesh->dumpIndices();

	auto vsize = sizeof(VERTEX_BASIC) * vcount;
	auto isize = sizeof(UINT) * icount;

	FillBuffer(dev, devcon, &sh_current->vb, vertices, vsize);
	FillBuffer(dev, devcon, &indexbuffer, indices, isize);

	setView(m_world, &mat_view, &mat_proj, diffuse, dev, devcon, constantbuffer);
	devcon->DrawIndexed(icount, 0, 0);

	setShader(sh_temp);
}
void Antimony::DrawMesh(VertexCompound *mesh, mat *m_world, bool ortho, color diffuse)
{
	assert(sh_current->ied == ied_main);

	UINT vcount = mesh->position.size();
	UINT icount = mesh->index.size();

	//VERTEX_BASIC *vertices = mesh_current->dumpBASIC();
	VERTEX_MAIN *vertices = mesh->dumpMAIN();
	UINT *indices = mesh->dumpIndices();

	auto vsize = sizeof(VERTEX_MAIN) * vcount;
	auto isize = sizeof(UINT) * icount;

	FillBuffer(dev, devcon, &sh_current->vb, vertices, vsize);
	FillBuffer(dev, devcon, &indexbuffer, indices, isize);

	setView(m_world, &mat_view, &mat_proj, diffuse, dev, devcon, constantbuffer);
	devcon->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	devcon->DrawIndexed(icount, 0, 0);

	if (false)
	{
		auto tempsh = sh_current;
		setShader(SHADERS_PLAIN);

		ID3D11RasterizerState *rss_curr;
		devcon->RSGetState(&rss_curr);
		devcon->RSSetState(rss_wireframe);
		setDepthBufferState(OFF);
		DrawMeshPlain(mesh, m_world, ortho, color(1, 1, 1, 0.3));
		setDepthBufferState(ON);
		DrawMeshPlain(mesh, m_world, ortho, color(1, 1, 1, 1));
		devcon->RSSetState(rss_curr);

		DrawNormals(vertices, vcount, &mesh->normalgroups, true);
		setShader(tempsh);
	}
}
void Antimony::DrawNormals(VERTEX_MAIN vin[], int vcount, ::std::vector<::std::vector<float3>> *normalgroups, bool components)
{
	auto sh_temp = sh_current;
	setShader(SHADERS_PLAIN);

	for (int v = 0; v < vcount; v++)
	{
		auto p = vin[v].position;
		auto n = vin[v].normal * 0.1;
		color c = color(n.x, n.y, n.z, 1);
		c = color(0.5, 0.5, 1, 1);

		XMVECTOR sv, rv, tv;
		XMMatrixDecompose(&sv, &rv, &tv, mat_world);
		mat r = XMMatrixRotationQuaternion(rv);

		p = (float3)XMVector3Transform(p, mat_world);
		n = (float3)XMVector3Transform(n, r);

		Draw3DLineThin(p, p + n, c, c, &mat_identity);
		auto _p = WorldToScreen(p + n);
		if (_p.z > 0)
		{
			if (true)
			{
				Draw2DDot(float2(_p.x, _p.y), 2, c);
				auto cfade = 255 * v / vcount;
				auto color = RGBA2DWORD(cfade, 0, 0, 255);
				color = RGBA2DWORD(128, 128, 255, 255);
				//auto txt = std::to_wstring(n.Length());
				auto txt = std::to_wstring(v);
				Consolas.render(txt.c_str(), 12, display.width / 2 + _p.x + 1, display.height / 2 + _p.y + 1, color, NULL);
			}
		}

		if (components)
			for (UINT nc = 0; nc < normalgroups->at(v).size(); nc++)
			{
				Draw3DLineThin(vin[v].position, vin[v].position + normalgroups->at(v).at(nc), COLOR_RED, COLOR_RED);
			}
	}
	setShader(sh_temp);
};

namespace Antimony
{
	IDXGISwapChain *swapchain;
	ID3D11Device *dev;
	ID3D11DeviceContext *devcon;

	ID3D11RenderTargetView *targettview;
	ID3D11DepthStencilView *depthstencilview;

	ID3D11RasterizerState *rss_standard, *rss_wireframe;
	ID3D11BlendState *blendstate;
	ID3D11DepthStencilState *dss_enabled, *dss_disabled;

	ID3D11Buffer *indexbuffer, *constantbuffer;

	Shader sh_main, sh_debug, sh_plain, sh_plain3D;
	Shader *sh_current = nullptr;

	mat mat_temp, mat_temp2, mat_world, mat_view, mat_proj, mat_orthoview, mat_orthoproj;

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
}
#include <string>
#include <fstream>
#include <vector>
#include <ctime>

#include "Shlwapi.h"

#include "Warnings.h"
#include "Window.h"
#include "Frame.h"
#include "DebugWin.h"

#pragma comment (lib, "Shlwapi.lib")

using namespace std;

HRESULT hr;

IDXGISwapChain *swapchain;
ID3D11Device *dev;
ID3D11DeviceContext *devcon;
ID3D11RenderTargetView *backbuffer;

ID3D11VertexShader *pVShader, *pCelVS, *pOutlineVS;
ID3D11PixelShader *pPShader, *pCelPS, *pOutlinePS;
ID3D11InputLayout *pLayout, *pCelLayout, *pOutlineLayout;

ID3D11Buffer *pVertexBuffer;
ID3D11Buffer *pIndexBuffer;
ID3D11Buffer *pConstantBuffer;

MatrixBufferType cameraMatrices;

//

float Timer::GetDelta()
{
	m_previousTime = m_currentTime;
	m_currentTime = clock();
	m_delta = abs(m_currentTime - m_previousTime) / m_correction;
	return m_delta;

	//overtime = clocktime - nextUpdate;
	//frametime = 0; //frames / (float)(1+ (float)framesteps/(float)CLOCKS_PER_SEC); // 0.00001 / (float)framesteps; //overtime;
}
void Timer::SetCorrection(float c)
{
	m_correction = c;
}
Timer::Timer()
{
	m_previousTime = 0;
	m_currentTime = 0;
	m_delta = 0;
	m_correction = CLOCKS_PER_SEC;
}
Timer timer;

//

HRESULT RenderFrameDX9(float delta)
{
	//
	return S_OK;
}

HRESULT RenderFrameDX11(float delta)
{
	PrepareFrame();
	UpdatePlayer(&keys, delta);
	UpdateCamera(&mouse, delta); // --> updates into mView

	devcon->IASetInputLayout(pLayout);
	devcon->VSSetShader(pVShader, 0, 0);
	devcon->PSSetShader(pPShader, 0, 0);

	//

	static float r = 0.3;

	/*VERTEX vertices[] =
	{
		{ r*(float)cos(-(D3DX_PI / 6)), r*(float)sin(-(D3DX_PI / 6)), 0.0f, color(1.0, 0.0, 0.0, 1.0) },
		{ r*(float)cos((D3DX_PI / 2)), r*(float)sin((D3DX_PI / 2)), 0.0f, color(0.0, 1.0, 0.0, 1.0) },
		{ r*(float)cos(7 * (D3DX_PI / 6)), r*(float)sin(7 * (D3DX_PI / 6)), 0.0f, color(0.0, 0.0, 1.0, 1.0) }
	};*/	
	VERTEX vertices[] =
	{
		{ -r, -r, -r, color(1.0, 1.0, 1.0, 1.0) },	//			7 o
		{ r, -r, -r, color(1.0, 1.0, 1.0, 1.0) },	//			  |		  6 o
		{ r, r, -r, color(1.0, 1.0, 1.0, 1.0) },	//	  3 o	  |			|
		{ -r, r, -r, color(1.0, 1.0, 1.0, 1.0) },	//		|	  |	2 o		|
		{ -r, -r, r, color(1.0, 1.0, 1.0, 1.0) },	//		|	4 o   |		|
		{ r, -r, r, color(1.0, 1.0, 1.0, 1.0) },	//		|		  |   5 o
		{ r, r, r, color(1.0, 1.0, 1.0, 1.0) },		//	  0 o		  |
		{ -r, r, r, color(1.0, 1.0, 1.0, 1.0) },	//				1 o
		//
		{ 0, 0, 0, color(0.0, 0.0, 0.0, 1.0) },
		{ 2, 0, 0, color(0.0, 0.0, 0.0, 1.0) },
		{ 2, 0, 2, color(0.0, 0.0, 0.0, 1.0) },
		{ 0, 0, 2, color(0.0, 0.0, 0.0, 1.0) },
		{ -2, 0, 2, color(0.0, 0.0, 0.0, 1.0) },
		{ -2, 0, 0, color(0.0, 0.0, 0.0, 1.0) },
		{ -2, 0, -2, color(0.0, 0.0, 0.0, 1.0) },
		{ 0, 0, -2, color(0.0, 0.0, 0.0, 1.0) },
		{ 2, 0, -2, color(0.0, 0.0, 0.0, 1.0) },
		//
		{ 0, 1, 0, color(0.0, 0.0, 0.0, 1.0) }
	};
	FillBuffer<VERTEX[]>(dev, devcon, &pVertexBuffer, vertices, sizeof(vertices));
	
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
		4, 6, 7,
		//
		10, 16,
		11, 15,
		12, 14,
		12, 10,
		13, 9,
		14, 16,
		//
		8, 17
	};
	FillBuffer<UINT[]>(dev, devcon, &pIndexBuffer, indices, sizeof(indices));

	static float h = 0;
	h += 0.5f * D3DX_PI * delta;
	if (h >= 2 * D3DX_PI)
		h = 0;
	D3DXMatrixRotationY(&mWorld, h);

	SetView(&mWorld, &mView, &mProj);
	devcon->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);	
	devcon->DrawIndexed(36, 0, 0);

	mWorld = mIdentity;
	SetView(&mWorld, &mView, &mProj);
	devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	devcon->DrawIndexed(14, 36, 0);

	//

	return PresentFrame();
}

HRESULT PrepareFrame()
{
	devcon->ClearRenderTargetView(backbuffer, color(0.0f, 0.2f, 0.4f, 1.0f));
	return S_OK;
}
HRESULT PresentFrame()
{
	if (wVSync)
		return swapchain->Present(1, 0);
	else
		return swapchain->Present(0, 0);
}

void UpdatePlayer(Keys* khandle, float delta)
{
	vec3 mov = vec3(0, 0, 0);
	float theta = camera.getAngle(th);

	if (khandle->forward.press > 0)
		mov = mov + vec3(cosf(theta), 0, sinf(theta));
	if (khandle->backward.press > 0)
		mov = mov + vec3(-cosf(theta), 0, -sinf(theta));
	if (khandle->left.press > 0)
		mov = mov + vec3(-sinf(theta), 0, cosf(theta));
	if (khandle->right.press > 0)
		mov = mov + vec3(sinf(theta), 0, -cosf(theta));

	player.moveToPoint(player.getPos() + mov * 3 * delta, -1);
}
void UpdateCamera(Mouse* mhandle, float delta)
{
	vec3 eye = origin;
	float slide = 0.004, radius = 1;
	static float
		theta = D3DX_PI / 2, // horizontal
		phi = D3DX_PI / 2, // vertical
		zoom = 0;

	// camera rotation
	if (mhandle->GetButtonState(rightbutton) == 0)
	{
		theta -= slide * mhandle->GetCoord(xcoord).vel;
		phi += slide * mhandle->GetCoord(ycoord).vel;
	}
	else // zoom
		zoom += float(mhandle->GetCoord(ycoord).vel) * 0.005;
	zoom -= float(mhandle->GetCoord(zcoord).vel) * 0.005;

	if (zoom < 0)
		zoom = 0;
	if (phi >= D3DX_PI - 0.001)
		phi = D3DX_PI - 0.001;
	if (phi <= 0.001)
		phi = 0.001;

	eye.x = (radius + zoom * zoom) * cosf(theta) * sinf(phi);
	eye.y = (radius + zoom * zoom) * cosf(phi);
	eye.z = (radius + zoom * zoom) * sinf(theta) * sinf(phi);

	vec3 h = vec3(0, 0.45, 0);

	// reset camera
	if (mhandle->GetButtonState(middlebutton) > 0)
	{
		camera.lookAtPoint(origin, .15);

		/*theta = D3DX_PI / 2;
		phi = D3DX_PI / 2;*/
	}

	camera.lookAtPoint(player.getPos() + h + eye, .5);
	camera.moveToPoint(player.getPos() + h - eye, .5);
	camera.setAngle(theta, phi);

	D3DXMatrixLookAtLH(&mView, &(camera.getPos()), &camera.getLookAt(), &vec3(0, 1, 0));
}
HRESULT SetView(mat *world, mat *view, mat *proj)
{
	MatrixBufferType matrices;

	matrices.world = TransposeMatrix(*world);
	matrices.view = TransposeMatrix(*view);
	matrices.projection = TransposeMatrix(*proj);

	return FillBuffer(dev, devcon, &pConstantBuffer, &matrices, sizeof(MatrixBufferType));
}
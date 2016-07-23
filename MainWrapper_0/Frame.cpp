#include <string>
#include <fstream>
#include <vector>
#include <ctime>

#include "Shlwapi.h"

#include "Warnings.h"
#include "Window.h"
#include "Frame.h"
#include "DebugWin.h"
#include "Controls.h"

#pragma comment (lib, "Shlwapi.lib")

using namespace std;

HRESULT hr;

IDXGISwapChain *swapchain;
ID3D11Device *dev;
ID3D11DeviceContext *devcon;

ID3D11RenderTargetView *targettview;
ID3D11DepthStencilView *depthstencilview;

ID3D11RasterizerState *rasterizerstate;
ID3D11DepthStencilState *depthstencil_enabled;
ID3D11DepthStencilState *depthstencil_disabled;

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
	PlayerControls(&keys, delta);
	CameraControls(&mouse, delta); // --> updates into mView

	devcon->IASetInputLayout(pLayout);
	devcon->VSSetShader(pVShader, 0, 0);
	devcon->PSSetShader(pPShader, 0, 0);

	//

	static float r = 0.3;

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

	//

	//SetDepthBufferState(OFF);

	mWorld = mIdentity;
	SetView(&mWorld, &mView, &mProj);
	devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	devcon->DrawIndexed(14, 36, 0); // axis

	//SetDepthBufferState(ON);

	static float h = 0;
	h += 0.5f * D3DX_PI * delta;
	if (h >= 2 * D3DX_PI)
		h = 0;
	D3DXMatrixRotationY(&mWorld, h);
	D3DXMatrixScaling(&mTemp, 0.5, 0.5, 0.5);
	mWorld = mTemp * mWorld;
	D3DXMatrixTranslation(&mTemp, 0, 1, 0);
	SetView(&(mTemp * mWorld), &mView, &mProj);
	devcon->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	devcon->DrawIndexed(36, 0, 0); // rotating cube

	D3DXMatrixTranslation(&mWorld, player.getPos().x, player.getPos().y + .3, player.getPos().z);
	D3DXMatrixScaling(&mTemp, 0.4, 1, 0.4);
	SetView(&(mTemp * mWorld), &mView, &mProj);
	devcon->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	devcon->DrawIndexed(36, 0, 0); // player cube

	//

	return PresentFrame();
}

HRESULT PrepareFrame()
{
	devcon->ClearRenderTargetView(targettview, color(0.0f, 0.2f, 0.4f, 0.0f));
	devcon->ClearDepthStencilView(depthstencilview, D3D11_CLEAR_DEPTH, 1.0f, 0);
	return S_OK;
}
HRESULT PresentFrame()
{
	if (wVSync)
		return swapchain->Present(1, 0);
	else
		return swapchain->Present(0, 0);
}

void PlayerControls(Keys* khandle, float delta)
{
	float speed = .3f;
	if (keys.sprint.press > 0)
		speed = .6f;

	vec3 mov = vec3(0, 0, 0);
	float th = camera.getAngle(theta);

	if (khandle->forward.press > 0)
		mov = mov + vec3(cosf(th), 0, sinf(th));
	if (khandle->backward.press > 0)
		mov = mov + vec3(-cosf(th), 0, -sinf(th));
	if (khandle->left.press > 0)
		mov = mov + vec3(-sinf(th), 0, cosf(th));
	if (khandle->right.press > 0)
		mov = mov + vec3(sinf(th), 0, -cosf(th));

	D3DXVec3Normalize(&mov, &mov);

	player.moveToPoint(player.getPos() + mov * speed, .1);

	player.update(delta);
}
void CameraControls(Mouse* mhandle, float delta)
{
	vec3 eye = origin;
	float slide = 0.004 * mSensibility, radius = 1;
	static float zoom = 1.2;
	static float _theta = D3DX_PI / 2;
	static float _phi = D3DX_PI / 2;

	// camera rotation
	if (mhandle->GetButtonState(rightbutton) == 0)
	{
		_theta -= (slide * mhandle->GetCoord(xcoord).vel) * (1 + 100 * delta);
		_phi += (slide * mhandle->GetCoord(ycoord).vel) * (1 + 100 * delta);
	}
	else // zoom
		zoom += float(mhandle->GetCoord(ycoord).vel) * 0.005;
	zoom -= float(mhandle->GetCoord(zcoord).vel) * 0.005;

	if (zoom < 0)
		zoom = 0;
	if (_phi >= D3DX_PI - 0.001)
		_phi = D3DX_PI - 0.001;
	if (_phi <= 0.001)
		_phi = 0.001;

	eye.x = (radius + zoom * zoom) * cosf(_theta) * sinf(_phi);
	eye.y = (radius + zoom * zoom) * cosf(_phi);
	eye.z = (radius + zoom * zoom) * sinf(_theta) * sinf(_phi);

	vec3 height = vec3(0, 0.75, 0);

	if (camera.isfree())
	{
		camera.lookAtPoint(player.getPos() + height + eye, .15);
		camera.moveToPoint(player.getPos() + height - eye * zoom, .145);
	}

	// reset camera
	if (mhandle->GetButtonState(middlebutton) > 0)
	{
		camera.lock();
		camera.lookAtPoint(origin, .15);
	}
	else if (!camera.isfree())
		camera.unlock();

	camera.update(delta);
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

void SetDepthBufferState(bool state)
{
	if (state == true)
		devcon->OMSetDepthStencilState(depthstencil_enabled, 1);
	else
		devcon->OMSetDepthStencilState(depthstencil_disabled, 1);
}
#include <string>
#include <fstream>
#include <vector>

#include "Warnings.h"
#include "Window.h"
#include "Frame.h"
#include "DebugWin.h"
#include "Controls.h"
#include "Gameflow.h"
#include "FBX.h"
#include "CpuRamUsage.h"
#include "Timer.h"
#include "Font.h"

#pragma comment (lib, "Shlwapi.lib")

using namespace std;

//

wchar_t global_str64[64];

//

char BoolToSign(bool b)
{
	if (b == false)
		return 1;
	if (b == true)
		return -1;
	return 0;
}

HRESULT Frame(double delta)
{
	PrepareFrame();

	if (ForGameState(GAMESTATE_INGAME))
	{
		UpdatePlayerControls(&keys, &controller[0], delta);
		UpdateCameraControls(&mouse, &keys, &controller[0], delta); // --> updates into mat_view
	}

	UpdateWorld(delta);
	UpdateAI(delta);
	UpdatePhysics(delta);
	UpdateHUD(delta);

	//

	RenderWorld();
	RenderEntities();
	RenderHUD();
	Render_Debug(delta);

	return PresentFrame();
}

HRESULT PrepareFrame()
{
	devcon->ClearRenderTargetView(targettview, RGBA{ 0.0f, 0.2f, 0.4f, 0.0f });
	devcon->ClearDepthStencilView(depthstencilview, D3D11_CLEAR_DEPTH, 1.0f, 0);

	UINT stride = sizeof(VERTEX_BASIC);
	UINT offset = 0;
	devcon->IASetVertexBuffers(0, 1, &vertexbuffer, &stride, &offset);
	devcon->IASetIndexBuffer(indexbuffer, DXGI_FORMAT_R32_UINT, 0);
	devcon->VSSetConstantBuffers(0, 1, &constantbuffer);

	return S_OK;
}
HRESULT PresentFrame()
{
	if (display.v_sync)
		return swapchain->Present(1, 0);
	else
		return swapchain->Present(0, 0);
}

void UpdatePlayerControls(KeysController *khandle, XInputController *xhandle, double delta)
{
	float speed = 4;
	if (khandle->sprint.GetState() > unpressed || xhandle->B.GetState() > unpressed)
		speed = 8;

	// = (player.getPosDest() - player.getPos()) / (speed * delta);
	float3 mmov, cmov;
	float th = camera.getAngle(theta);

	if (khandle->forward.GetState() > unpressed)
		mmov += float3(cosf(th), 0, sinf(th));
	if (khandle->backward.GetState() > unpressed)
		mmov += float3(-cosf(th), 0, -sinf(th));
	if (khandle->left.GetState() > unpressed)
		mmov += float3(-sinf(th), 0, cosf(th));
	if (khandle->right.GetState() > unpressed)
		mmov += float3(sinf(th), 0, -cosf(th));
	
	cmov = float3(sinf(th) * xhandle->LX.vel + cosf(th) * xhandle->LY.vel, 0, sinf(th) * xhandle->LY.vel + cosf(th) * (-xhandle->LX.vel));
	
	float3 mov = mmov + cmov;
	if (mov.Length() > 1)
		mov = XMVector3Normalize(mov);	

	player.moveToPoint(player.getPosDest() + mov * speed * delta, .999999971);

	player.update(delta);
}
void UpdateCameraControls(MouseController *mhandle, KeysController *khandle, XInputController *xhandle, double delta)
{
	float3 eye =  v3_origin;
	float mSlide = 0.15 * mSensibility * (delta + 0.01 * (1 - delta));		// it's... complicated.
	float xSlide = 4 * xSensibility * delta;
	float radius = 1;
	float maxpitch = 0.1;
	static float zoom = 1.2;
	static float _theta = DX_PI / 2;
	static float _phi = DX_PI / 2;

	// camera rotation
	if (khandle->RMB.GetState() == unpressed)
	{
		_theta -= mSlide * mhandle->X.vel * BoolToSign(mouseXAxis)
			+ xSlide * xhandle->RX.vel * BoolToSign(controllerXAxis);
		_phi += mSlide * mhandle->Y.vel * BoolToSign(mouseYAxis)
			+ xSlide * xhandle->RY.vel * -BoolToSign(controllerYAxis);
	}
	else // zoom
		zoom += float(mhandle->Y.vel) * 0.005;
	zoom -= float(mhandle->Z.vel) * 0.005;

	if (zoom < 0)
		zoom = 0;
	if (_phi >= DX_PI - maxpitch)
		_phi = DX_PI - maxpitch;
	if (_phi <= maxpitch)
		_phi = maxpitch;

	eye.x = (radius + zoom * zoom) * cosf(_theta) * sinf(_phi);
	eye.y = (radius + zoom * zoom) * cosf(_phi);
	eye.z = (radius + zoom * zoom) * sinf(_theta) * sinf(_phi);

	float3 height = float3(0, 0.75, 0);

	if (camera.isfree())
		camera.lookAtPoint(player.getPos() + height + eye, game.camera_friction * (.99999999) + !game.camera_friction);

	camera.moveToPoint(player.getPos() + height - eye * zoom, game.camera_friction * (.9999999) + !game.camera_friction);

	// reset camera
	if (khandle->MMB.GetState() == held || xhandle->RS.GetState() == held)
	{
		camera.lock();
		camera.lookAtPoint(v3_origin, .99999);
		zoom = 1.2;
	}
	else if (!camera.isfree())
		camera.unlock();

	camera.update(delta);

	mat_view = MLookAtLH(camera.getPos(), camera.getLookAt(), float3(0, 1, 0));
}

void UpdateHUD(double delta)
{
	// TODO: Implement font/text printing
	// TODO: Implement HUD
}
void UpdateAI(double delta)
{
	// TODO: Implement AI
}
void UpdatePhysics(double delta)
{
	// TODO: Implement physics
}
void UpdateWorld(double delta)
{
	// TODO: Implement world mechanics
	// TODO: Implement triggers
}

void RenderWorld()
{
	//
}
void RenderEntities()
{
	//
}
void RenderHUD()
{
	//
}
void Render_Debug(double delta)
{
	SetShader(SHADERS_DEBUG);

	static float h = 0;
	h += 0.5f * DX_PI * delta;
	if (h >= 2 * DX_PI)
		h = 0;
	mat_world = MRotY(h);

	if (0)
	{
		unsigned int f = 0;
		f = abs(10 * cosf(h));
		Sleep(f);
	}

	Draw3DCube(0.15, color(2, 1, 1, 1), &(MTranslation(0, 1, 0) * mat_world));

	Draw3DLineThin(float3(-2, 0, -2), float3(-2, 0, 2), COLOR_BLACK, COLOR_BLACK, &mat_identity);
	Draw3DLineThin(float3(0, 0, -2), float3(0, 0, 2), COLOR_BLACK, COLOR_BLACK, &mat_identity);
	Draw3DLineThin(float3(2, 0, -2), float3(2, 0, 2), COLOR_BLACK, COLOR_BLACK, &mat_identity);
	Draw3DLineThin(float3(-2, 0, -2), float3(2, 0, -2), COLOR_BLACK, COLOR_BLACK, &mat_identity);
	Draw3DLineThin(float3(-2, 0, 0), float3(2, 0, 0), COLOR_BLACK, COLOR_BLACK, &mat_identity);
	Draw3DLineThin(float3(-2, 0, 2), float3(2, 0, 2), COLOR_BLACK, COLOR_BLACK, &mat_identity);
	Draw3DLineThin(float3(0, 0, 0), float3(0, 1, 0), COLOR_BLACK, COLOR_BLACK, &mat_identity);

	mat_world = MTranslVector(player.getPos() + float3(0, 0.3, 0));
	Draw3DBox(0.15, 0.3, 0.15, COLOR_WHITE);

	//

	SetDepthBufferState(OFF);
	SetShader(SHADERS_PLAIN);

	Render_DebugKeyboard(float2(0, 300));
	Render_DebugMouse(float2(0, 300) + float2(90, 30));
	Render_DebugController(float2(0, 300), 0);
	Render_DebugFPS(float2(windowMain.width * 0.5, -windowMain.height * 0.5));

	SetDepthBufferState(ON);
}
void Render_DebugKeyboard(float2 pos)
{
	VERTEX_BASIC vertices[] =
	{
		{ -.5, 0, 0, COLOR_WHITE },
		{ .5, 0, 0, COLOR_WHITE },
		{ .5, 1, 0, COLOR_WHITE },
		{ -.5, 1, 0, COLOR_WHITE },
		// 4
		{ -1.44, -1, 0, COLOR_WHITE },
		{ -1.75, 1, 0, COLOR_WHITE },
		{ -.8, 1, 0, COLOR_WHITE },
		{ -.56, .25, 0, COLOR_WHITE },
		{ .56, .25, 0, COLOR_WHITE },
		{ .8, 1, 0, COLOR_WHITE },
		{ 1.75, 1, 0, COLOR_WHITE },
		{ 1.44, -1, 0, COLOR_WHITE }
		// 12
	};
	FillBuffer<VERTEX_BASIC[]>(dev, devcon, &vertexbuffer, vertices, sizeof(vertices));

	UINT indices[] =
	{
		0, 2, 1,
		0, 3, 2,
		// 6
		4, 5, 6,
		4, 6, 7,
		4, 7, 8,
		4, 8, 11,
		11, 8, 9,
		11, 9, 10
		// 24
	};
	FillBuffer<UINT[]>(dev, devcon, &indexbuffer, indices, sizeof(indices));

	devcon->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	mat_world = MScaling(2200, 2200, 1) * MTranslation(pos.x, pos.y, 0);

	// Esc
	mat_temp = MScaling(0.01, 0.009, 1) * MTranslation(0.107775, -0.090, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// F1
	mat_temp = MScaling(0.009, 0.009, 1) * MTranslation(0.124775, -0.090, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// F2
	mat_temp = MScaling(0.009, 0.009, 1) * MTranslation(0.137775, -0.090, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// F3
	mat_temp = MScaling(0.009, 0.009, 1) * MTranslation(0.150775, -0.090, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// F4
	mat_temp = MScaling(0.009, 0.009, 1) * MTranslation(0.163775, -0.090, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// F5
	mat_temp = MScaling(0.009, 0.009, 1) * MTranslation(0.179775, -0.090, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// F6
	mat_temp = MScaling(0.009, 0.009, 1) * MTranslation(0.192775, -0.090, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// F7
	mat_temp = MScaling(0.009, 0.009, 1) * MTranslation(0.205775, -0.090, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// F8
	mat_temp = MScaling(0.009, 0.009, 1) * MTranslation(0.217775, -0.090, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// F9
	mat_temp = MScaling(0.009, 0.009, 1) * MTranslation(0.234775, -0.090, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// F10
	mat_temp = MScaling(0.009, 0.009, 1) * MTranslation(0.247775, -0.090, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// F11
	mat_temp = MScaling(0.009, 0.009, 1) * MTranslation(0.260775, -0.090, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// F12
	mat_temp = MScaling(0.009, 0.009, 1) * MTranslation(0.273775, -0.090, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);

	// Ins
	mat_temp = MScaling(0.009, 0.008, 1) * MTranslation(0.28875, -0.078, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// Start
	mat_temp = MScaling(0.009, 0.008, 1) * MTranslation(0.30275, -0.078, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// Pgup
	mat_temp = MScaling(0.009, 0.008, 1) * MTranslation(0.31675, -0.078, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// Canc
	mat_temp = MScaling(0.009, 0.008, 1) * MTranslation(0.28875, -0.090, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// End
	mat_temp = MScaling(0.009, 0.008, 1) * MTranslation(0.30275, -0.090, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// Pgdn
	mat_temp = MScaling(0.009, 0.008, 1) * MTranslation(0.31675, -0.090, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);

	// Backslash
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.107775, -0.105, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// 1
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.122775, -0.105, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// 2
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.137775, -0.105, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// 3
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.152775, -0.105, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// 4
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.167775, -0.105, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// 5
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.182775, -0.105, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// 6
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.197775, -0.105, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// 7
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.212775, -0.105, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// 8
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.227775, -0.105, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// 9
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.242775, -0.105, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// 0
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.257775, -0.105, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// '
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.272775, -0.105, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// �
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.287775, -0.105, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// Backspace
	mat_temp = MScaling(0.026, 0.01, 1) * MTranslation(0.3105, -0.105, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);

	// Tab
	mat_temp = MScaling(0.0195, 0.01, 1) * MTranslation(0.11275, -0.12, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// Q
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.1325, -0.12, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// W
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.1475, -0.12, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, BtnStateColor(keys.forward));
	devcon->DrawIndexed(6, 0, 0);
	// E
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.1625, -0.12, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, BtnStateColor(keys.action));
	devcon->DrawIndexed(6, 0, 0);
	// R
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.1775, -0.12, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// T
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.1925, -0.12, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// Y
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.2075, -0.12, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// U
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.2225, -0.12, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// I
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.2375, -0.12, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// O
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.2525, -0.12, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// P
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.2675, -0.12, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// �
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.2825, -0.12, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// +
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.2975, -0.12, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// Enter
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.3125, -0.12, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);

	// CAPS
	mat_temp = MScaling(0.022, 0.01, 1) * MTranslation(0.114, -0.135, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// A
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.135, -0.135, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, BtnStateColor(keys.left));
	devcon->DrawIndexed(6, 0, 0);
	// S
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.15, -0.135, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, BtnStateColor(keys.backward));
	devcon->DrawIndexed(6, 0, 0);
	// D
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.165, -0.135, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, BtnStateColor(keys.right));
	devcon->DrawIndexed(6, 0, 0);
	// F
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.18, -0.135, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// G
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.195, -0.135, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// H
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.21, -0.135, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// J
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.225, -0.135, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// K
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.24, -0.135, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// L
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.255, -0.135, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// �
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.27, -0.135, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// �
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.285, -0.135, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// �
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.3, -0.135, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// Enter
	mat_temp = MScaling(0.014, 0.025, 1) * MTranslation(0.317, -0.135, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);

	// Left Shift
	mat_temp = MScaling(0.014, 0.01, 1) * MTranslation(0.11, -0.15, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, BtnStateColor(keys.sprint));
	devcon->DrawIndexed(6, 0, 0);
	// < >
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.1275, -0.15, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// Z
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.1425, -0.15, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// X
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.1575, -0.15, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// C
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.1725, -0.15, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// V
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.1875, -0.15, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// B
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.2025, -0.15, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// N
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.2175, -0.15, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// M
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.2325, -0.15, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// ,
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.2475, -0.15, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// .
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.2625, -0.15, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// -
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.2775, -0.15, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// Right Shift
	mat_temp = MScaling(0.036, 0.01, 1) * MTranslation(0.3055, -0.15, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);

	// Left Ctrl
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.10775, -0.165, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// Fn
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.12275, -0.165, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// Start
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.13775, -0.165, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// Left Alt
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.15275, -0.165, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// Spacebar
	mat_temp = MScaling(0.07, 0.01, 1) * MTranslation(0.19775, -0.165, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, BtnStateColor(keys.jump));
	devcon->DrawIndexed(6, 0, 0);
	// Alt Gr
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.24275, -0.165, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// Win
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.25775, -0.165, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// Right Ctrl
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.27275, -0.165, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);

	// Up
	mat_temp = MScaling(0.01, 0.008, 1) * MTranslation(0.30275, -0.163, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// Left
	mat_temp = MScaling(0.01, 0.008, 1) * MTranslation(0.28875, -0.175, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// Down
	mat_temp = MScaling(0.01, 0.008, 1) * MTranslation(0.30275, -0.175, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
	// Right
	mat_temp = MScaling(0.01, 0.008, 1) * MTranslation(0.31675, -0.175, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
}
void Render_DebugMouse(float2 pos)
{
	VERTEX_BASIC vertices[] =
	{
		{ -.5, 0, 0, COLOR_WHITE },
		{ .5, 0, 0, COLOR_WHITE },
		{ .5, 1, 0, COLOR_WHITE },
		{ -.5, 1, 0, COLOR_WHITE },
		// 4
		{ -1.44, -1, 0, COLOR_WHITE },
		{ -1.75, 1, 0, COLOR_WHITE },
		{ -.8, 1, 0, COLOR_WHITE },
		{ -.56, .25, 0, COLOR_WHITE },
		{ .56, .25, 0, COLOR_WHITE },
		{ .8, 1, 0, COLOR_WHITE },
		{ 1.75, 1, 0, COLOR_WHITE },
		{ 1.44, -1, 0, COLOR_WHITE }
		// 12
	};
	FillBuffer<VERTEX_BASIC[]>(dev, devcon, &vertexbuffer, vertices, sizeof(vertices));

	UINT indices[] =
	{
		0, 2, 1,
		0, 3, 2,
		// 6
		4, 5, 6,
		4, 6, 7,
		4, 7, 8,
		4, 8, 11,
		11, 8, 9,
		11, 9, 10
		// 24
	};
	FillBuffer<UINT[]>(dev, devcon, &indexbuffer, indices, sizeof(indices));

	devcon->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	mat_world = MScaling(2200, 2200, 1) * MTranslation(pos.x, pos.y, 0);

	// Left
	mat_temp = MScaling(0.0075, 0.01, 1) * MTranslation(0.1, -0.050, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, BtnStateColor(keys.LMB));
	devcon->DrawIndexed(6, 0, 0);

	// Middle
	mat_temp = MScaling(0.0035, 0.01, 1) * MTranslation(0.11, -0.050, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, BtnStateColor(keys.MMB));
	devcon->DrawIndexed(6, 0, 0);

	// Right
	mat_temp = MScaling(0.0075, 0.01, 1) * MTranslation(0.12, -0.050, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, BtnStateColor(keys.RMB));
	devcon->DrawIndexed(6, 0, 0);

	// Mouse
	mat_temp = MScaling(0.028, 0.0235, 1) * MTranslation(0.11, -0.0775, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(6, 0, 0);
}
void Render_DebugController(float2 pos, unsigned char c)
{
	VERTEX_BASIC vertices[] =
	{
		{ -.5, 0, 0, COLOR_WHITE },
		{ .5, 0, 0, COLOR_WHITE },
		{ .5, 1, 0, COLOR_WHITE },
		{ -.5, 1, 0, COLOR_WHITE },
		// 4
		{ -1.44, -1, 0, COLOR_WHITE },
		{ -1.75, 1, 0, COLOR_WHITE },
		{ -.8, 1, 0, COLOR_WHITE },
		{ -.56, .25, 0, COLOR_WHITE },
		{ .56, .25, 0, COLOR_WHITE },
		{ .8, 1, 0, COLOR_WHITE },
		{ 1.75, 1, 0, COLOR_WHITE },
		{ 1.44, -1, 0, COLOR_WHITE }
		// 12
	};
	FillBuffer<VERTEX_BASIC[]>(dev, devcon, &vertexbuffer, vertices, sizeof(vertices));

	UINT indices[] =
	{
		0, 2, 1,
		0, 3, 2,
		// 6
		4, 5, 6,
		4, 6, 7,
		4, 7, 8,
		4, 8, 11,
		11, 8, 9,
		11, 9, 10
		// 24
	};
	FillBuffer<UINT[]>(dev, devcon, &indexbuffer, indices, sizeof(indices));

	devcon->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	mat_world = MScaling(2200, 2200, 1) * MTranslation(pos.x, pos.y, 0);

	// Layout
	mat_temp = MRotZ(DX_PI) * MScaling(0.0175, 0.0175, 1) * MTranslation(0.23, -0.050, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, color(.4, .4, .4, 1));
	devcon->DrawIndexed(18, 6, 0);

	// Back
	mat_temp = MScaling(0.0045, 0.004, 1) * MTranslation(0.225, -0.045, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, BtnStateColor(controller[c].Back));
	devcon->DrawIndexed(6, 0, 0);

	// Start
	mat_temp = MScaling(0.0045, 0.004, 1) * MTranslation(0.235, -0.045, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, BtnStateColor(controller[c].Start));
	devcon->DrawIndexed(6, 0, 0);

	// Left
	mat_temp = MScaling(0.005, 0.005, 1) * MTranslation(0.2075, -0.045, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, BtnStateColor(controller[c].Left));
	devcon->DrawIndexed(6, 0, 0);

	// Right
	mat_temp = MScaling(0.005, 0.005, 1) * MTranslation(0.2175, -0.045, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, BtnStateColor(controller[c].Right));
	devcon->DrawIndexed(6, 0, 0);

	// Down
	mat_temp = MScaling(0.005, 0.005, 1) * MTranslation(0.2125, -0.05, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, BtnStateColor(controller[c].Down));
	devcon->DrawIndexed(6, 0, 0);

	// Up
	mat_temp = MScaling(0.005, 0.005, 1) * MTranslation(0.2125, -0.04, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, BtnStateColor(controller[c].Up));
	devcon->DrawIndexed(6, 0, 0);

	// X
	mat_temp = MScaling(0.005, 0.005, 1) * MTranslation(0.2425, -0.045, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, BtnStateColor(controller[c].X));
	devcon->DrawIndexed(6, 0, 0);

	// B
	mat_temp = MScaling(0.005, 0.005, 1) * MTranslation(0.2525, -0.045, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, BtnStateColor(controller[c].B));
	devcon->DrawIndexed(6, 0, 0);

	// A
	mat_temp = MScaling(0.005, 0.005, 1) * MTranslation(0.2475, -0.05, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, BtnStateColor(controller[c].A));
	devcon->DrawIndexed(6, 0, 0);

	// Y
	mat_temp = MScaling(0.005, 0.005, 1) * MTranslation(0.2475, -0.04, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, BtnStateColor(controller[c].Y));
	devcon->DrawIndexed(6, 0, 0);

	// LT
	mat_temp = MScaling(0.0075, 0.0075, 1) * MTranslation(0.2125, -0.0325, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, BtnStateColor(controller[c].LT));
	devcon->DrawIndexed(6, 0, 0);

	// RT
	mat_temp = MScaling(0.0075, 0.0075, 1) * MTranslation(0.2475, -0.0325, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, BtnStateColor(controller[c].RT));
	devcon->DrawIndexed(6, 0, 0);

	// LB
	mat_temp = MScaling(0.01, 0.0025, 1) * MTranslation(0.2125, -0.0325, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, BtnStateColor(controller[c].LB));
	devcon->DrawIndexed(6, 0, 0);

	// RB
	mat_temp = MScaling(0.01, 0.0025, 1) * MTranslation(0.2475, -0.0325, 0);
	SetView(&(mat_temp * mat_world), &mat_orthoview, &mat_orthoproj, BtnStateColor(controller[c].RB));
	devcon->DrawIndexed(6, 0, 0);
	
	// Left Circle
	Draw2DEllipses(15.4, 15.4, pos.x + 487.3, pos.y - 116.6, color(.4, .4, .4, 1));

	// Right Circle
	Draw2DEllipses(15.4, 15.4, pos.x + 524.7, pos.y - 116.6, color(.4, .4, .4, 1));

	// Left Stick
	Draw2DEllipses(9.9, 9.9, pos.x + 487.3 + controller[c].LX.vel * 4.4, pos.y - 116.6 + controller[c].LY.vel * 4.4, BtnStateColor(controller[c].LS));

	// Right Stick
	Draw2DEllipses(9.9, 9.9, pos.x + 524.7 + controller[c].RX.vel * 4.4, pos.y - 116.6 + controller[c].RY.vel * 4.4, BtnStateColor(controller[c].RS));

}
void Render_DebugFPS(float2 pos)
{
	short cpu = cpu_usage.GetUsage(0);

	K32GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
	physMemUsedByMe = pmc.WorkingSetSize;
	double physMemPercent = double(physMemUsedByMe) / double(totalPhysMem) * 100;
	double physMemMB = double(physMemUsedByMe) / 1000000;

	color fps_bar;
	int fps_line = 0;
	if (timer.GetFramesCount() < 256)
	{
		fps_bar = color((float)(255 - timer.GetFramesCount()) / (float)255, (float)(timer.GetFramesCount()) / (float)255, 0, 1);
		fps_line = 0.7843 * timer.GetFramesCount() + pos.x - 260;
	}
	else
	{
		fps_bar = color(0, (float)(510 - timer.GetFramesCount()) / (float)255, (float)(timer.GetFramesCount() - 255) / (float)255, 1);
		fps_line = 0.7843 * 256 + pos.x - 260;
	}
	Draw2DRectangle(200, 10, pos.x - 260, pos.y + 174, color(0, 0, 0, 0.2));
	Draw2DLineThick(float2(pos.x - 260, pos.y + 179), float2(fps_line, pos.y + 179), 10, fps_bar, fps_bar);
	Draw2DRectBorderThick(200, 10, pos.x - 260, pos.y + 174, 1, color(0, 0.73, 0.73, 1));
	Draw2DLineThick(float2(pos.x - 252.157, pos.y + 174), float2(pos.x - 252.157, pos.y + 184), 1, color(0, 0.73, 0.73, 1), color(0, 0.73, 0.73, 1)); // 10 FPS
	Draw2DLineThick(float2(pos.x - 236.471, pos.y + 174), float2(pos.x - 236.471, pos.y + 184), 1, color(0, 0.73, 0.73, 1), color(0, 0.73, 0.73, 1)); // 30 FPS
	Draw2DLineThick(float2(pos.x - 212.942, pos.y + 174), float2(pos.x - 212.942, pos.y + 184), 1, color(0, 0.73, 0.73, 1), color(0, 0.73, 0.73, 1)); // 60 FPS
	Draw2DLineThick(float2(pos.x - 181.57, pos.y + 174), float2(pos.x - 181.57, pos.y + 184), 1, color(0, 0.73, 0.73, 1), color(0, 0.73, 0.73, 1)); // 100 FPS
	Draw2DLineThick(float2(pos.x - 103.14, pos.y + 174), float2(pos.x - 103.14, pos.y + 184), 1, color(0, 0.73, 0.73, 1), color(0, 0.73, 0.73, 1)); // 200 FPS

	Draw2DRectangle(200, 54, pos.x - 260, pos.y + 98, color(0, 0, 0, 0.2));
	for (unsigned int i = 1; i < cpu_usage.usageStream.size(); i++)
	{
		Draw2DLineThin(
			float2(pos.x - 260 + 2 * (i - 1) * ((float)100 / ((float)cpu_usage.GetMaxRecords() - 1)), pos.y + 100 + (float)cpu_usage.usageStream.at(i - 1) * 0.5),
			float2(pos.x - 260 + 2 * (i) * ((float)100 / ((float)cpu_usage.GetMaxRecords() - 1)), pos.y + 100 + (float)cpu_usage.usageStream.at(i) * 0.5),
			color(1, 0.06, 0.6, 1), color(1, 0.06, 0.6, 1));
	}
	Draw2DRectBorderThick(200, 54, pos.x - 260, pos.y + 98, 1, color(0, 0.73, 0.73, 1));

	Draw2DRectangle(200, 10, pos.x - 260, pos.y + 83, color(0, 0, 0, 0.2));
	Draw2DLineThick(float2(pos.x - 260, pos.y + 88), float2(1.945 * cpu + pos.x - 258, pos.y + 88), 10, color(1, 0.06, 0.6, 1), color(1, 0.06, 0.6, 1));
	Draw2DRectBorderThick(200, 10, pos.x - 260, pos.y + 83, 1, color(0, 0.73, 0.73, 1));
	
	/*sprintf(global_text, "CPU usage: %d%%", cpu4);
	PrintText(global_text, Tiny, pos.x - 260, pos.y + 78, D3DCOLOR_XRGB(255, 255, 255));
	sprintf(global_text, "RAM usage: %.2f%%", physMemPercent);
	PrintText(global_text, Tiny, pos.x - 260, pos.y + 64, D3DCOLOR_XRGB(255, 255, 255));
	sprintf(global_text, "Alloc.: %.3f MB", physMemMB);
	PrintText(global_text, Tiny, pos.x - 260, pos.y + 50, D3DCOLOR_XRGB(255, 255, 255));*/

	swprintf(global_str64, L"FPS: %.2f (%i)", timer.GetFPSStamp(), timer.GetFramesCount());
	fw_courier->DrawString(devcon, global_str64, 10, windowMain.width - 260, windowMain.height - 200, 0xffffffff, 0);
	swprintf(global_str64, L"Max: %.2f", timer.maxFps);
	fw_courier->DrawString(devcon, global_str64, 10, windowMain.width - 125, windowMain.height - 200, 0xffffffff, 0);

	swprintf(global_str64, L"CPU usage: %d%%", cpu);
	fw_courier->DrawString(devcon, global_str64, 10, windowMain.width - 260, windowMain.height - 78, 0xffffffff, 0);
	swprintf(global_str64, L"RAM usage: %.2f%%", physMemPercent);
	fw_courier->DrawString(devcon, global_str64, 10, windowMain.width - 260, windowMain.height - 64, 0xffffffff, 0);
	swprintf(global_str64, L"Alloc.: %.3f MB", physMemMB);
	fw_courier->DrawString(devcon, global_str64, 10, windowMain.width - 260, windowMain.height - 50, 0xffffffff, 0);
}

void SetDepthBufferState(bool state)
{
	if (state == true)
		devcon->OMSetDepthStencilState(dss_enabled, 1);
	else
		devcon->OMSetDepthStencilState(dss_disabled, 1);
}

color BtnStateColor(Input bt)
{
	switch (bt.GetState())
	{
	case unpressed:
		return color(1, 1, 1, 1);
	case pressed:
		return color(0, 1, 0, 1);
	case held:
		return color(0, 0, 1, 1);
	case released:
		return color(1, 0, 0, 1);
	default:
		return color(1, 1, 1, 1);
	}
}
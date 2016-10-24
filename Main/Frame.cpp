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
#include "Gameflow.h"
#include "FBX.h"

#pragma comment (lib, "Shlwapi.lib")

using namespace std;

HRESULT hr;

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

	UpdateHUD(delta);
	UpdateAI(delta);
	UpdatePhysics(delta);
	UpdateWorld(delta);
	Update_Debug(delta);

	//

	RenderWorld();
	RenderEntities();
	RenderHUD();
	Render_Debug();

	return PresentFrame();
}

HRESULT PrepareFrame()
{
	devcon->ClearRenderTargetView(targettview, RGBA{ 0.0f, 0.2f, 0.4f, 0.0f });
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

HRESULT SetView(mat *world, mat *view, mat *proj, color diffuse = color(1, 1, 1, 1))
{
	ConstantBuffer buffer;

	buffer.world = TransposeMatrix(*world);
	buffer.view = TransposeMatrix(*view);
	buffer.projection = TransposeMatrix(*proj);
	buffer.diffuse = diffuse;

	return FillBuffer(dev, devcon, &constantbuffer, &buffer, sizeof(buffer));
}

void UpdatePlayerControls(KeysController *khandle, XInputController *xhandle, double delta)
{
	float speed = 2;
	if (khandle->sprint.GetState() > unpressed || xhandle->B.GetState() > unpressed)
		speed = 4;

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

	player.moveToPoint(player.getPosDest() + mov * speed * delta, .9999999);

	player.update(delta);
}
void UpdateCameraControls(MouseController *mhandle, KeysController *khandle, XInputController *xhandle, double delta)
{
	float3 eye =  v_origin;
	float mSlide = 0.003 * mSensibility;
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
		zoom += float(mhandle->Z.vel) * 0.005;
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
		camera.lookAtPoint(player.getPos() + height + eye, .99999999);

	camera.moveToPoint(player.getPos() + height - eye * zoom, .99999999);

	// reset camera
	if (khandle->MMB.GetState() == held || xhandle->RS.GetState() == held)
	{
		camera.lock();
		camera.lookAtPoint(v_origin, .99999);
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
void Update_Debug(double delta)
{
	static float r = 0.3;

	VERTEX_BASIC vertices[] =
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
		{ 0, 1, 0, color(0.0, 0.0, 0.0, 1.0) },
		//
		{ -.5, 0, 0, color(1.0, 1.0, 1.0, 1.0) },
		{ .5, 0, 0, color(1.0, 1.0, 1.0, 1.0) },
		{ .5, 1, 0, color(1.0, 1.0, 1.0, 1.0) },
		{ -.5, 1, 0, color(1.0, 1.0, 1.0, 1.0) },
		//
		{ -1.44, -1, 0, color(1.0, 1.0, 1.0, 1.0) },
		{ -1.75, 1, 0, color(1.0, 1.0, 1.0, 1.0) },
		{ -.8, 1, 0, color(1.0, 1.0, 1.0, 1.0) },
		{ -.56, .25, 0, color(1.0, 1.0, 1.0, 1.0) },
		{ .56, .25, 0, color(1.0, 1.0, 1.0, 1.0) },
		{ .8, 1, 0, color(1.0, 1.0, 1.0, 1.0) },
		{ 1.75, 1, 0, color(1.0, 1.0, 1.0, 1.0) },
		{ 1.44, -1, 0, color(1.0, 1.0, 1.0, 1.0) },
		//
		{ cosf(0 * DX_PI / 20), sinf(0 * DX_PI / 20), 0, color(1.0, 1.0, 1.0, 1.0) },
		{ cosf(1 * DX_PI / 20), sinf(1 * DX_PI / 20), 0, color(1.0, 1.0, 1.0, 1.0) },
		{ cosf(2 * DX_PI / 20), sinf(2 * DX_PI / 20), 0, color(1.0, 1.0, 1.0, 1.0) },
		{ cosf(3 * DX_PI / 20), sinf(3 * DX_PI / 20), 0, color(1.0, 1.0, 1.0, 1.0) },
		{ cosf(4 * DX_PI / 20), sinf(4 * DX_PI / 20), 0, color(1.0, 1.0, 1.0, 1.0) },
		{ cosf(5 * DX_PI / 20), sinf(5 * DX_PI / 20), 0, color(1.0, 1.0, 1.0, 1.0) },
		{ cosf(6 * DX_PI / 20), sinf(6 * DX_PI / 20), 0, color(1.0, 1.0, 1.0, 1.0) },
		{ cosf(7 * DX_PI / 20), sinf(7 * DX_PI / 20), 0, color(1.0, 1.0, 1.0, 1.0) },
		{ cosf(8 * DX_PI / 20), sinf(8 * DX_PI / 20), 0, color(1.0, 1.0, 1.0, 1.0) },
		{ cosf(9 * DX_PI / 20), sinf(9 * DX_PI / 20), 0, color(1.0, 1.0, 1.0, 1.0) },
		{ cosf(10 * DX_PI / 20), sinf(10 * DX_PI / 20), 0, color(1.0, 1.0, 1.0, 1.0) },
		{ cosf(11 * DX_PI / 20), sinf(11 * DX_PI / 20), 0, color(1.0, 1.0, 1.0, 1.0) },
		{ cosf(12 * DX_PI / 20), sinf(12 * DX_PI / 20), 0, color(1.0, 1.0, 1.0, 1.0) },
		{ cosf(13 * DX_PI / 20), sinf(13 * DX_PI / 20), 0, color(1.0, 1.0, 1.0, 1.0) },
		{ cosf(14 * DX_PI / 20), sinf(14 * DX_PI / 20), 0, color(1.0, 1.0, 1.0, 1.0) },
		{ cosf(15 * DX_PI / 20), sinf(15 * DX_PI / 20), 0, color(1.0, 1.0, 1.0, 1.0) },
		{ cosf(16 * DX_PI / 20), sinf(16 * DX_PI / 20), 0, color(1.0, 1.0, 1.0, 1.0) },
		{ cosf(17 * DX_PI / 20), sinf(17 * DX_PI / 20), 0, color(1.0, 1.0, 1.0, 1.0) },
		{ cosf(18 * DX_PI / 20), sinf(18 * DX_PI / 20), 0, color(1.0, 1.0, 1.0, 1.0) },
		{ cosf(19 * DX_PI / 20), sinf(19 * DX_PI / 20), 0, color(1.0, 1.0, 1.0, 1.0) },
		{ cosf(20 * DX_PI / 20), sinf(20 * DX_PI / 20), 0, color(1.0, 1.0, 1.0, 1.0) },
		{ cosf(21 * DX_PI / 20), sinf(21 * DX_PI / 20), 0, color(1.0, 1.0, 1.0, 1.0) },
		{ cosf(22 * DX_PI / 20), sinf(22 * DX_PI / 20), 0, color(1.0, 1.0, 1.0, 1.0) },
		{ cosf(23 * DX_PI / 20), sinf(23 * DX_PI / 20), 0, color(1.0, 1.0, 1.0, 1.0) },
		{ cosf(24 * DX_PI / 20), sinf(24 * DX_PI / 20), 0, color(1.0, 1.0, 1.0, 1.0) },
		{ cosf(25 * DX_PI / 20), sinf(25 * DX_PI / 20), 0, color(1.0, 1.0, 1.0, 1.0) },
		{ cosf(26 * DX_PI / 20), sinf(26 * DX_PI / 20), 0, color(1.0, 1.0, 1.0, 1.0) },
		{ cosf(27 * DX_PI / 20), sinf(27 * DX_PI / 20), 0, color(1.0, 1.0, 1.0, 1.0) },
		{ cosf(28 * DX_PI / 20), sinf(28 * DX_PI / 20), 0, color(1.0, 1.0, 1.0, 1.0) },
		{ cosf(29 * DX_PI / 20), sinf(29 * DX_PI / 20), 0, color(1.0, 1.0, 1.0, 1.0) },
		{ cosf(30 * DX_PI / 20), sinf(30 * DX_PI / 20), 0, color(1.0, 1.0, 1.0, 1.0) },
		{ cosf(31 * DX_PI / 20), sinf(31 * DX_PI / 20), 0, color(1.0, 1.0, 1.0, 1.0) },
		{ cosf(32 * DX_PI / 20), sinf(32 * DX_PI / 20), 0, color(1.0, 1.0, 1.0, 1.0) },
		{ cosf(33 * DX_PI / 20), sinf(33 * DX_PI / 20), 0, color(1.0, 1.0, 1.0, 1.0) },
		{ cosf(34 * DX_PI / 20), sinf(34 * DX_PI / 20), 0, color(1.0, 1.0, 1.0, 1.0) },
		{ cosf(35 * DX_PI / 20), sinf(35 * DX_PI / 20), 0, color(1.0, 1.0, 1.0, 1.0) },
		{ cosf(36 * DX_PI / 20), sinf(36 * DX_PI / 20), 0, color(1.0, 1.0, 1.0, 1.0) },
		{ cosf(37 * DX_PI / 20), sinf(37 * DX_PI / 20), 0, color(1.0, 1.0, 1.0, 1.0) },
		{ cosf(38 * DX_PI / 20), sinf(38 * DX_PI / 20), 0, color(1.0, 1.0, 1.0, 1.0) },
		{ cosf(39 * DX_PI / 20), sinf(39 * DX_PI / 20), 0, color(1.0, 1.0, 1.0, 1.0) },
		{ cosf(40 * DX_PI / 20), sinf(40 * DX_PI / 20), 0, color(1.0, 1.0, 1.0, 1.0) },
		{ 0, 0, 0, color(1.0, 1.0, 1.0, 1.0) }
	};
	FillBuffer<VERTEX_BASIC[]>(dev, devcon, &vertexbuffer, vertices, sizeof(vertices));

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
		8, 17,
		//
		18, 20, 19,
		18, 21, 20,
		//
		22, 23, 24,
		22, 24, 25,
		22, 25, 26,
		22, 26, 29,
		29, 26, 27,
		29, 27, 28,
		//
		30, 31, 71,
		31, 32, 71,
		32, 33, 71,
		33, 34, 71,
		34, 35, 71,
		35, 36, 71,
		36, 37, 71,
		37, 38, 71,
		38, 39, 71,
		39, 40, 71,
		40, 41, 71,
		41, 42, 71,
		42, 43, 71,
		43, 44, 71,
		44, 45, 71,
		45, 46, 71,
		46, 47, 71,
		47, 48, 71,
		48, 49, 71,
		49, 50, 71,
		50, 51, 71,
		51, 52, 71,
		52, 53, 71,
		53, 54, 71,
		54, 55, 71,
		55, 56, 71,
		56, 57, 71,
		57, 58, 71,
		58, 59, 71,
		59, 60, 71,
		60, 61, 71,
		61, 62, 71,
		62, 63, 71,
		63, 64, 71,
		64, 65, 71,
		65, 66, 71,
		66, 67, 71,
		67, 68, 71,
		68, 69, 71,
		69, 70, 71,
		70, 30, 71
	};
	FillBuffer<UINT[]>(dev, devcon, &indexbuffer, indices, sizeof(indices));

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
}

void RenderWorld()
{

}
void RenderEntities()
{

}
void RenderHUD()
{

}
void Render_Debug()
{
	devcon->IASetInputLayout(il_debug);
	devcon->VSSetShader(vs_debug, 0, 0);
	devcon->PSSetShader(ps_debug, 0, 0);

	mat_temp = MScaling(0.5, 0.5, 0.5);
	mat_world = mat_temp * mat_world;
	mat_temp = MTranslation(0, 1, 0);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj);
	devcon->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	devcon->DrawIndexed(36, 0, 0); // rotating cube

	mat_world = mat_identity;
	SetView(&mat_world, &mat_view, &mat_proj);
	devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	devcon->DrawIndexed(14, 36, 0); // axis

	mat_world = MTranslVector(player.getPos() + float3(0, 0.3, 0));
	SetView(&(MScaling(0.4, 1, 0.4) * mat_world), &mat_view, &mat_proj);
	devcon->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	devcon->DrawIndexed(36, 0, 0); // player cube

	//mat_world = MTranslVector(player.getPosDest() + float3(0, 0.3, 0));
	//SetView(&(MScaling(0.4, 1, 0.4) * mat_world), &mat_view, &mat_proj);
	//devcon->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//devcon->DrawIndexed(36, 0, 0); // dest cube

	//

	SetDepthBufferState(OFF);

	Render_DebugKeyboard(v_origin);
	Render_DebugMouse(float3(0.02, 0.01, 0));
	Render_DebugController(v_origin, 0);

	SetDepthBufferState(ON);
}
void Render_DebugKeyboard(float3 pos)
{
	mat_world = MRotX(camera.getAngle(phi) + DX_PI * 1.5) * MRotY(-camera.getAngle(theta) + DX_PI * .5)
		* MTranslVector(camera.getPos() + 0.1 * (camera.getLookAt() - camera.getPos()));

	// Esc
	mat_temp = MScaling(0.01, 0.009, 1) * MTranslation(0.107775, -0.090, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, color(.2, .2, .2, 1));
	devcon->DrawIndexed(6, 50, 0);
	// F1
	mat_temp = MScaling(0.009, 0.009, 1) * MTranslation(0.124775, -0.090, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, color(.2, .2, .2, 1));
	devcon->DrawIndexed(6, 50, 0);
	// F2
	mat_temp = MScaling(0.009, 0.009, 1) * MTranslation(0.137775, -0.090, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, color(.2, .2, .2, 1));
	devcon->DrawIndexed(6, 50, 0);
	// F3
	mat_temp = MScaling(0.009, 0.009, 1) * MTranslation(0.150775, -0.090, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, color(.2, .2, .2, 1));
	devcon->DrawIndexed(6, 50, 0);
	// F4
	mat_temp = MScaling(0.009, 0.009, 1) * MTranslation(0.163775, -0.090, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, color(.2, .2, .2, 1));
	devcon->DrawIndexed(6, 50, 0);
	// F5
	mat_temp = MScaling(0.009, 0.009, 1) * MTranslation(0.179775, -0.090, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, color(.2, .2, .2, 1));
	devcon->DrawIndexed(6, 50, 0);
	// F6
	mat_temp = MScaling(0.009, 0.009, 1) * MTranslation(0.192775, -0.090, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, color(.2, .2, .2, 1));
	devcon->DrawIndexed(6, 50, 0);
	// F7
	mat_temp = MScaling(0.009, 0.009, 1) * MTranslation(0.205775, -0.090, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, color(.2, .2, .2, 1));
	devcon->DrawIndexed(6, 50, 0);
	// F8
	mat_temp = MScaling(0.009, 0.009, 1) * MTranslation(0.217775, -0.090, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, color(.2, .2, .2, 1));
	devcon->DrawIndexed(6, 50, 0);
	// F9
	mat_temp = MScaling(0.009, 0.009, 1) * MTranslation(0.234775, -0.090, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, color(.2, .2, .2, 1));
	devcon->DrawIndexed(6, 50, 0);
	// F10
	mat_temp = MScaling(0.009, 0.009, 1) * MTranslation(0.247775, -0.090, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, color(.2, .2, .2, 1));
	devcon->DrawIndexed(6, 50, 0);
	// F11
	mat_temp = MScaling(0.009, 0.009, 1) * MTranslation(0.260775, -0.090, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, color(.2, .2, .2, 1));
	devcon->DrawIndexed(6, 50, 0);
	// F12
	mat_temp = MScaling(0.009, 0.009, 1) * MTranslation(0.273775, -0.090, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, color(.2, .2, .2, 1));
	devcon->DrawIndexed(6, 50, 0);

	// Ins
	mat_temp = MScaling(0.009, 0.008, 1) * MTranslation(0.28875, -0.078, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, color(.2, .2, .2, 1));
	devcon->DrawIndexed(6, 50, 0);
	// Start
	mat_temp = MScaling(0.009, 0.008, 1) * MTranslation(0.30275, -0.078, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, color(.2, .2, .2, 1));
	devcon->DrawIndexed(6, 50, 0);
	// Pgup
	mat_temp = MScaling(0.009, 0.008, 1) * MTranslation(0.31675, -0.078, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, color(.2, .2, .2, 1));
	devcon->DrawIndexed(6, 50, 0);
	// Canc
	mat_temp = MScaling(0.009, 0.008, 1) * MTranslation(0.28875, -0.090, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, color(.2, .2, .2, 1));
	devcon->DrawIndexed(6, 50, 0);
	// End
	mat_temp = MScaling(0.009, 0.008, 1) * MTranslation(0.30275, -0.090, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, color(.2, .2, .2, 1));
	devcon->DrawIndexed(6, 50, 0);
	// Pgdn
	mat_temp = MScaling(0.009, 0.008, 1) * MTranslation(0.31675, -0.090, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, color(.2, .2, .2, 1));
	devcon->DrawIndexed(6, 50, 0);

	// Backslash
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.107775, -0.105, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, color(.2, .2, .2, 1));
	devcon->DrawIndexed(6, 50, 0);
	// 1
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.122775, -0.105, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, color(.2, .2, .2, 1));
	devcon->DrawIndexed(6, 50, 0);
	// 2
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.137775, -0.105, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, color(.2, .2, .2, 1));
	devcon->DrawIndexed(6, 50, 0);
	// 3
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.152775, -0.105, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, color(.2, .2, .2, 1));
	devcon->DrawIndexed(6, 50, 0);
	// 4
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.167775, -0.105, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, color(.2, .2, .2, 1));
	devcon->DrawIndexed(6, 50, 0);
	// 5
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.182775, -0.105, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, color(.2, .2, .2, 1));
	devcon->DrawIndexed(6, 50, 0);
	// 6
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.197775, -0.105, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, color(.2, .2, .2, 1));
	devcon->DrawIndexed(6, 50, 0);
	// 7
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.212775, -0.105, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, color(.2, .2, .2, 1));
	devcon->DrawIndexed(6, 50, 0);
	// 8
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.227775, -0.105, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, color(.2, .2, .2, 1));
	devcon->DrawIndexed(6, 50, 0);
	// 9
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.242775, -0.105, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, color(.2, .2, .2, 1));
	devcon->DrawIndexed(6, 50, 0);
	// 0
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.257775, -0.105, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, color(.2, .2, .2, 1));
	devcon->DrawIndexed(6, 50, 0);
	// '
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.272775, -0.105, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, color(.2, .2, .2, 1));
	devcon->DrawIndexed(6, 50, 0);
	// ì
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.287775, -0.105, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, color(.2, .2, .2, 1));
	devcon->DrawIndexed(6, 50, 0);
	// Backspace
	mat_temp = MScaling(0.026, 0.01, 1) * MTranslation(0.3105, -0.105, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, color(.2, .2, .2, 1));
	devcon->DrawIndexed(6, 50, 0);

	// Tab
	mat_temp = MScaling(0.0195, 0.01, 1) * MTranslation(0.11275, -0.12, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, color(.2, .2, .2, 1));
	devcon->DrawIndexed(6, 50, 0);
	// Q
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.1325, -0.12, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, color(.2, .2, .2, 1));
	devcon->DrawIndexed(6, 50, 0);
	// W
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.1475, -0.12, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, BtnStateColor(keys.forward));
	devcon->DrawIndexed(6, 50, 0);
	// E
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.1625, -0.12, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, BtnStateColor(keys.action));
	devcon->DrawIndexed(6, 50, 0);
	// R
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.1775, -0.12, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, color(.2, .2, .2, 1));
	devcon->DrawIndexed(6, 50, 0);
	// T
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.1925, -0.12, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, color(.2, .2, .2, 1));
	devcon->DrawIndexed(6, 50, 0);
	// Y
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.2075, -0.12, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, color(.2, .2, .2, 1));
	devcon->DrawIndexed(6, 50, 0);
	// U
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.2225, -0.12, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, color(.2, .2, .2, 1));
	devcon->DrawIndexed(6, 50, 0);
	// I
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.2375, -0.12, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, color(.2, .2, .2, 1));
	devcon->DrawIndexed(6, 50, 0);
	// O
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.2525, -0.12, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, color(.2, .2, .2, 1));
	devcon->DrawIndexed(6, 50, 0);
	// P
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.2675, -0.12, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, color(.2, .2, .2, 1));
	devcon->DrawIndexed(6, 50, 0);
	// è
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.2825, -0.12, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, color(.2, .2, .2, 1));
	devcon->DrawIndexed(6, 50, 0);
	// +
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.2975, -0.12, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, color(.2, .2, .2, 1));
	devcon->DrawIndexed(6, 50, 0);
	// Enter
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.3125, -0.12, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, color(.2, .2, .2, 1));
	devcon->DrawIndexed(6, 50, 0);

	// CAPS
	mat_temp = MScaling(0.022, 0.01, 1) * MTranslation(0.114, -0.135, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, color(.2, .2, .2, 1));
	devcon->DrawIndexed(6, 50, 0);
	// A
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.135, -0.135, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, BtnStateColor(keys.left));
	devcon->DrawIndexed(6, 50, 0);
	// S
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.15, -0.135, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, BtnStateColor(keys.backward));
	devcon->DrawIndexed(6, 50, 0);
	// D
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.165, -0.135, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, BtnStateColor(keys.right));
	devcon->DrawIndexed(6, 50, 0);
	// F
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.18, -0.135, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, color(.2, .2, .2, 1));
	devcon->DrawIndexed(6, 50, 0);
	// G
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.195, -0.135, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, color(.2, .2, .2, 1));
	devcon->DrawIndexed(6, 50, 0);
	// H
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.21, -0.135, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, color(.2, .2, .2, 1));
	devcon->DrawIndexed(6, 50, 0);
	// J
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.225, -0.135, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, color(.2, .2, .2, 1));
	devcon->DrawIndexed(6, 50, 0);
	// K
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.24, -0.135, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, color(.2, .2, .2, 1));
	devcon->DrawIndexed(6, 50, 0);
	// L
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.255, -0.135, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, color(.2, .2, .2, 1));
	devcon->DrawIndexed(6, 50, 0);
	// ò
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.27, -0.135, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, color(.2, .2, .2, 1));
	devcon->DrawIndexed(6, 50, 0);
	// à
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.285, -0.135, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, color(.2, .2, .2, 1));
	devcon->DrawIndexed(6, 50, 0);
	// ù
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.3, -0.135, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, color(.2, .2, .2, 1));
	devcon->DrawIndexed(6, 50, 0);
	// Enter
	mat_temp = MScaling(0.014, 0.025, 1) * MTranslation(0.317, -0.135, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, color(.2, .2, .2, 1));
	devcon->DrawIndexed(6, 50, 0);

	// Left Shift
	mat_temp = MScaling(0.014, 0.01, 1) * MTranslation(0.11, -0.15, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, BtnStateColor(keys.sprint));
	devcon->DrawIndexed(6, 50, 0);
	// < >
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.1275, -0.15, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, color(.2, .2, .2, 1));
	devcon->DrawIndexed(6, 50, 0);
	// Z
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.1425, -0.15, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, color(.2, .2, .2, 1));
	devcon->DrawIndexed(6, 50, 0);
	// X
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.1575, -0.15, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, color(.2, .2, .2, 1));
	devcon->DrawIndexed(6, 50, 0);
	// C
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.1725, -0.15, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, color(.2, .2, .2, 1));
	devcon->DrawIndexed(6, 50, 0);
	// V
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.1875, -0.15, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, color(.2, .2, .2, 1));
	devcon->DrawIndexed(6, 50, 0);
	// B
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.2025, -0.15, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, color(.2, .2, .2, 1));
	devcon->DrawIndexed(6, 50, 0);
	// N
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.2175, -0.15, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, color(.2, .2, .2, 1));
	devcon->DrawIndexed(6, 50, 0);
	// M
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.2325, -0.15, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, color(.2, .2, .2, 1));
	devcon->DrawIndexed(6, 50, 0);
	// ,
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.2475, -0.15, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, color(.2, .2, .2, 1));
	devcon->DrawIndexed(6, 50, 0);
	// .
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.2625, -0.15, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, color(.2, .2, .2, 1));
	devcon->DrawIndexed(6, 50, 0);
	// -
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.2775, -0.15, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, color(.2, .2, .2, 1));
	devcon->DrawIndexed(6, 50, 0);
	// Right Shift
	mat_temp = MScaling(0.036, 0.01, 1) * MTranslation(0.3055, -0.15, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, color(.2, .2, .2, 1));
	devcon->DrawIndexed(6, 50, 0);

	// Left Ctrl
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.10775, -0.165, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, color(.2, .2, .2, 1));
	devcon->DrawIndexed(6, 50, 0);
	// Fn
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.12275, -0.165, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, color(.2, .2, .2, 1));
	devcon->DrawIndexed(6, 50, 0);
	// Start
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.13775, -0.165, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, color(.2, .2, .2, 1));
	devcon->DrawIndexed(6, 50, 0);
	// Left Alt
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.15275, -0.165, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, color(.2, .2, .2, 1));
	devcon->DrawIndexed(6, 50, 0);
	// Spacebar
	mat_temp = MScaling(0.07, 0.01, 1) * MTranslation(0.19775, -0.165, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, BtnStateColor(keys.jump));
	devcon->DrawIndexed(6, 50, 0);
	// Alt Gr
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.24275, -0.165, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, color(.2, .2, .2, 1));
	devcon->DrawIndexed(6, 50, 0);
	// Win
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.25775, -0.165, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, color(.2, .2, .2, 1));
	devcon->DrawIndexed(6, 50, 0);
	// Right Ctrl
	mat_temp = MScaling(0.01, 0.01, 1) * MTranslation(0.27275, -0.165, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, color(.2, .2, .2, 1));
	devcon->DrawIndexed(6, 50, 0);

	// Up
	mat_temp = MScaling(0.01, 0.008, 1) * MTranslation(0.30275, -0.163, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, color(.2, .2, .2, 1));
	devcon->DrawIndexed(6, 50, 0);
	// Left
	mat_temp = MScaling(0.01, 0.008, 1) * MTranslation(0.28875, -0.175, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, color(.2, .2, .2, 1));
	devcon->DrawIndexed(6, 50, 0);
	// Down
	mat_temp = MScaling(0.01, 0.008, 1) * MTranslation(0.30275, -0.175, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, color(.2, .2, .2, 1));
	devcon->DrawIndexed(6, 50, 0);
	// Right
	mat_temp = MScaling(0.01, 0.008, 1) * MTranslation(0.31675, -0.175, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, color(.2, .2, .2, 1));
	devcon->DrawIndexed(6, 50, 0);
}
void Render_DebugMouse(float3 pos)
{
	mat_world = MRotX(camera.getAngle(phi) + DX_PI * 1.5) * MRotY(-camera.getAngle(theta) + DX_PI * .5)
		* MTranslVector(camera.getPos() + 0.1 * (camera.getLookAt() - camera.getPos()));

	// Left
	mat_temp = MScaling(0.0075, 0.01, 1) * MTranslation(0.1, -0.050, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, BtnStateColor(keys.LMB));
	devcon->DrawIndexed(6, 50, 0);

	// Middle
	mat_temp = MScaling(0.0035, 0.01, 1) * MTranslation(0.11, -0.050, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, BtnStateColor(keys.MMB));
	devcon->DrawIndexed(6, 50, 0);

	// Right
	mat_temp = MScaling(0.0075, 0.01, 1) * MTranslation(0.12, -0.050, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, BtnStateColor(keys.RMB));
	devcon->DrawIndexed(6, 50, 0);

	// Mouse
	mat_temp = MScaling(0.028, 0.0235, 1) * MTranslation(0.11, -0.0775, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, color(.2, .2, .2, 1));
	devcon->DrawIndexed(6, 50, 0);
}
void Render_DebugController(float3 pos, unsigned char c)
{
	mat_world = MRotX(camera.getAngle(phi) + DX_PI * 1.5) * MRotY(-camera.getAngle(theta) + DX_PI * .5)
		* MTranslVector(camera.getPos() + 0.1 * (camera.getLookAt() - camera.getPos()));

	// Layout
	mat_temp = MRotZ(DX_PI) * MScaling(0.0175, 0.0175, 1) * MTranslation(0.23, -0.050, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, color(.2, .2, .2, 1));
	devcon->DrawIndexed(18, 56, 0);

	// Left Circle
	mat_temp = MScaling(0.007, -0.007, 1) * MTranslation(0.2215, -0.053, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, color(.2, .2, .2, 1));
	devcon->DrawIndexed(120, 74, 0);

	// Right Circle
	mat_temp = MScaling(0.007, -0.007, 1) * MTranslation(0.2385, -0.053, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, color(.2, .2, .2, 1));
	devcon->DrawIndexed(120, 74, 0);

	// Left Stick
	mat_temp = MScaling(0.0045, -0.0045, 1) * MTranslation(0.2215, -0.053, 0) * MTranslation(pos.x, pos.y, pos.z) * MTranslation(controller[c].LX.vel * 0.002, controller[c].LY.vel * 0.002, 0);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, BtnStateColor(controller[c].LS));
	devcon->DrawIndexed(120, 74, 0);

	// Right Stick
	mat_temp = MScaling(0.0045, -0.0045, 1) * MTranslation(0.2385, -0.053, 0) * MTranslation(pos.x, pos.y, pos.z) * MTranslation(controller[c].RX.vel * 0.002, controller[c].RY.vel * 0.002, 0);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, BtnStateColor(controller[c].RS));
	devcon->DrawIndexed(120, 74, 0);

	// Back
	mat_temp = MScaling(0.0045, 0.004, 1) * MTranslation(0.225, -0.045, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, BtnStateColor(controller[c].Back));
	devcon->DrawIndexed(6, 50, 0);

	// Start
	mat_temp = MScaling(0.0045, 0.004, 1) * MTranslation(0.235, -0.045, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, BtnStateColor(controller[c].Start));
	devcon->DrawIndexed(6, 50, 0);

	// Left
	mat_temp = MScaling(0.005, 0.005, 1) * MTranslation(0.2075, -0.045, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, BtnStateColor(controller[c].Left));
	devcon->DrawIndexed(6, 50, 0);

	// Right
	mat_temp = MScaling(0.005, 0.005, 1) * MTranslation(0.2175, -0.045, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, BtnStateColor(controller[c].Right));
	devcon->DrawIndexed(6, 50, 0);

	// Down
	mat_temp = MScaling(0.005, 0.005, 1) * MTranslation(0.2125, -0.05, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, BtnStateColor(controller[c].Down));
	devcon->DrawIndexed(6, 50, 0);

	// Up
	mat_temp = MScaling(0.005, 0.005, 1) * MTranslation(0.2125, -0.04, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, BtnStateColor(controller[c].Up));
	devcon->DrawIndexed(6, 50, 0);

	// X
	mat_temp = MScaling(0.005, 0.005, 1) * MTranslation(0.2425, -0.045, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, BtnStateColor(controller[c].X));
	devcon->DrawIndexed(6, 50, 0);

	// B
	mat_temp = MScaling(0.005, 0.005, 1) * MTranslation(0.2525, -0.045, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, BtnStateColor(controller[c].B));
	devcon->DrawIndexed(6, 50, 0);

	// A
	mat_temp = MScaling(0.005, 0.005, 1) * MTranslation(0.2475, -0.05, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, BtnStateColor(controller[c].A));
	devcon->DrawIndexed(6, 50, 0);

	// Y
	mat_temp = MScaling(0.005, 0.005, 1) * MTranslation(0.2475, -0.04, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, BtnStateColor(controller[c].Y));
	devcon->DrawIndexed(6, 50, 0);

	// LT
	mat_temp = MScaling(0.0075, 0.0075, 1) * MTranslation(0.2125, -0.0325, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, BtnStateColor(controller[c].LT));
	devcon->DrawIndexed(6, 50, 0);

	// RT
	mat_temp = MScaling(0.0075, 0.0075, 1) * MTranslation(0.2475, -0.0325, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, BtnStateColor(controller[c].RT));
	devcon->DrawIndexed(6, 50, 0);

	// LB
	mat_temp = MScaling(0.01, 0.0025, 1) * MTranslation(0.2125, -0.0325, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, BtnStateColor(controller[c].LB));
	devcon->DrawIndexed(6, 50, 0);

	// RB
	mat_temp = MScaling(0.01, 0.0025, 1) * MTranslation(0.2475, -0.0325, 0) * MTranslation(pos.x, pos.y, pos.z);
	SetView(&(mat_temp * mat_world), &mat_view, &mat_proj, BtnStateColor(controller[c].RB));
	devcon->DrawIndexed(6, 50, 0);
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
#include "Antimony.h"
#include "Timer.h"
#include "CpuUsage.h"
#include "Gameflow.h"
#include "Player.h"
#include "Camera.h"

///

char BoolToSign(bool b)
{
	if (b == false)
		return 1;
	if (b == true)
		return -1;
	return 0;
}

void Antimony::step()
{
	timer.UpdateDelta(TIMER_FRAME_GLOBAL);
	m_delta = timer.GetDelta(TIMER_FRAME_GLOBAL);
	double fstep = m_delta * m_worldSpeed;

	updateGameState();

	if (ifGameState(GAMESTATE_INGAME))									// INGAME (non-paused, non-menu etc.)
	{
		updateWorld(fstep);												// update moving objects, triggers etc. (TBI)
		updatePlayerControls(&keys, &controller[0], fstep);				// update player inputs
		updatePhysics(fstep);											// btWorld step

		updateCameraControls(&mouse, &keys, &controller[0], fstep);		// update camera (--> mat_view)
		updateAI(fstep);												// update AI/scripts etc. (TBI)
	}

	prepareFrame();														// prepare the frame for rendering

	render_Debug();														// render debug info
}
void Antimony::endStep()
{
	presentFrame();														// present frame to the GPU

	if (game.debug)
		consoleLog();													// log debugging info to the console
	mouse.reset();
	keys.reset();
	for (unsigned char i = 0; i< XUSER_MAX_COUNT; i++)
	{
		if (controller[i].isEnabled())
			controller[i].reset();
	}
}
HRESULT Antimony::prepareFrame()
{
	devcon->ClearRenderTargetView(targettview, RGBA{ 0.0f, 0.2f, 0.4f, 0.0f });			// clear the render target view
	devcon->ClearDepthStencilView(depthstencilview, D3D11_CLEAR_DEPTH, 1.0f, 0);		// clear depth stencil

	devcon->IASetVertexBuffers(0, 1, &vertexbuffer, &vertex_stride, &vertex_offset);	// set vertex buffer
	devcon->IASetIndexBuffer(indexbuffer, DXGI_FORMAT_R32_UINT, 0);						// set index buffer
	devcon->VSSetConstantBuffers(0, 1, &constantbuffer);								// set constant buffer

	return S_OK;
}
HRESULT Antimony::presentFrame()
{
	if (display.vsync)
		return swapchain->Present(1, 0);
	else
		return swapchain->Present(0, DXGI_PRESENT_DO_NOT_WAIT);
	frame_count++;
}
void Antimony::updatePlayerControls(KeysController *khandle, XInputController *xhandle, double delta)
{
	float speed = 4 * player.m_movSpeed;
	if (khandle->sprint.getState() > BTN_UNPRESSED || xhandle->B.getState() > BTN_UNPRESSED)
		speed = 8 * player.m_movSpeed;

	float3 mmov, xmov;
	float th = camera.getAngle(CAM_THETA);

	if (khandle->forward.getState() > BTN_UNPRESSED)
		mmov += float3(cosf(th), 0, sinf(th));
	if (khandle->backward.getState() > BTN_UNPRESSED)
		mmov += float3(-cosf(th), 0, -sinf(th));
	if (khandle->left.getState() > BTN_UNPRESSED)
		mmov += float3(-sinf(th), 0, cosf(th));
	if (khandle->right.getState() > BTN_UNPRESSED)
		mmov += float3(sinf(th), 0, -cosf(th));

	xmov = float3(sinf(th) * xhandle->LX.getVel() + cosf(th) * xhandle->LY.getVel(), 0, sinf(th) * xhandle->LY.getVel() + cosf(th) * (-xhandle->LX.getVel()));

	float3 mov = mmov + xmov;
	if (mov.Length() > 1)
		mov = XMVector3Normalize(mov);

	//player.MoveToPoint(player.GetPosDest() + mov * speed * delta, .999999971);

	player.update(delta, m_objectsCollisions);
	player.move(&Float3Tobt(&mov), speed);

	if (khandle->jump.getState() > BTN_UNPRESSED || xhandle->A.getState() > BTN_UNPRESSED)
		player.attemptJump();
}
void Antimony::updateCameraControls(MouseController *mhandle, KeysController *khandle, XInputController *xhandle, double delta)
{
	float3 eye = v3_origin;
	float m_slide = 0.005 * controls.m_sensitivity;
	float x_slide = 6 * controls.x_sensitivity * delta;
	float radius = 1;
	float maxpitch = 0.1;
	static float zoom = 1.2;
	static float _theta = DX_PI / 2;
	static float _phi = DX_PI / 2;

	// camera rotation
	if (mhandle->RMB.getState() == BTN_UNPRESSED && xhandle->LT.getState() < BTN_PRESSED)
	{
		_theta -= m_slide * mhandle->X.getVel() * BoolToSign(controls.m_invertxaxis)
			+ x_slide * xhandle->RX.getVel() * BoolToSign(controls.x_invertxaxis);
		_phi += m_slide * mhandle->Y.getVel() * BoolToSign(controls.m_invertyaxis)
			+ x_slide * xhandle->RY.getVel() * -BoolToSign(controls.x_invertyaxis);
	}
	else if (mhandle->RMB.getState() > BTN_UNPRESSED || xhandle->LT.getState() > BTN_UNPRESSED) // zoom
	{
		zoom += float(mhandle->Y.getVel()) * 0.005;
		zoom -= float(xhandle->RY.getVel()) * 0.01;
	}
	zoom -= float(mhandle->Z.getVel()) * 0.005;

	if (zoom < 0)
		zoom = 0;
	if (_phi >= DX_PI - maxpitch)
		_phi = DX_PI - maxpitch;
	if (_phi <= maxpitch)
		_phi = maxpitch;

	eye.x = (radius + zoom * zoom) * cosf(_theta) * sinf(_phi);
	eye.y = (radius + zoom * zoom) * cosf(_phi);
	eye.z = (radius + zoom * zoom) * sinf(_theta) * sinf(_phi);

	float3 height = v3_origin;//float3(0, 0.75, 0);

	float3 clook = MatToFloat3(&m_physEntities.at(game.dbg_entityfollow%m_physEntities.size())->getMatTransform());//player.getPos();

	if (camera.isFree())
		camera.lookAtPoint(clook + WORLD_SCALE * (height + eye), game.camera_friction * (.99999999) + !game.camera_friction);
	//camera.LookAtPoint(clook + WORLD_SCALE * (height), -1);

	camera.moveToPoint(clook + WORLD_SCALE * (height - eye * zoom), game.camera_friction * (.9999999) + !game.camera_friction);
	//camera.MoveToPoint(camera.GetLookAt() + WORLD_SCALE * (height - eye * zoom), -1);

	// reset camera
	if (mhandle->MMB.getState() == BTN_HELD || xhandle->RS.getState() == BTN_HELD)
	{
		camera.lock();
		//camera.LookAtPoint(v3_origin, .99999);
		camera.lookAtPoint(player.getColl()->getFlat3Pos(), .99999);
		zoom = 1.2;
	}
	else if (!camera.isFree())
		camera.unlock();

	camera.update(delta);

	mat_view = MLookAtLH(camera.getPos(), camera.getLookAt(), float3(0, 1, 0));
}
void Antimony::updateAI(double delta)
{
	// TODO: Implement AI
}
void Antimony::updatePhysics(double delta)
{
	// TODO: Implement physics

	m_btWorld->stepSimulation(delta, 10, 1.f / 240.f);
}
void Antimony::updateWorld(double delta)
{
	// TODO: Implement world mechanics
	// TODO: Implement triggers

	static double h = 0;
	if (ifGameState(GAMESTATE_INGAME))
		h += 0.5f * DX_PI * delta;
	if (h >= 2 * DX_PI)
		h = 0;

	if (0)
	{
		unsigned int f = 0;
		f = abs(50 * cosf(h) - 40);
		Sleep(f);
	}

	m_physEntities.at(3)->setMatTransform(&(MTranslation(WORLD_SCALE * 0, WORLD_SCALE * 0.5, WORLD_SCALE * 0) * MRotY(h)));
	m_physEntities.at(4)->setMatTransform(&(MTranslation(WORLD_SCALE * 3, WORLD_SCALE * 1, WORLD_SCALE * sinf(h))));
	m_physEntities.at(4)->updateKinematic(delta);
	m_physEntities.at(5)->setMatTransform(&(MTranslation(WORLD_SCALE * 3, WORLD_SCALE * (1 - 0.5 * sinf(h)), 2)));
	//m_physEntities.at(5)->updateKinematic(delta);
	//m_physEntities.at(5)->getRigidBody()->setLinearVelocity(btVector3(0, WORLD_SCALE * (sinf(h)), 0));
	//m_physEntities.at(5)->getRigidBody()->setLinearFactor(btVector3(0, WORLD_SCALE * (sinf(h)), 0));
	//m_physEntities.at(5)->getRigidBody()->setLinearVelocity(bt_origin);
}
void Antimony::updateGameState()
{
	if (ifGameState(GAMESTATE_INGAME))									// INGAME (non-paused, non-menu etc.)
	{
		if (keys.sk_escape.getState() == BTN_PRESSED)
			//SetGameState(GAMESTATE_PAUSE);
			PostQuitMessage(0);
	}
	else if (ifGameState(GAMESTATE_PAUSE))								// PAUSED (in-antimony.game inventory/pause menu)
	{
		if (keys.sk_escape.getState() == BTN_PRESSED)
		{
			setGameState(GAMESTATE_INGAME);
			//PostQuitMessage(0);
		}
	}
}
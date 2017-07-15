#include "Antimony.h"
#include "Timer.h"
#include "Gameflow.h"

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

	if (GetFocus() == window_main.hWnd)
	{
		ShowCursor(false);
		ClipCursor(&window_main.plane);
	}

	updateGameState();

	if (ifGameState(GAMESTATE_INGAME))									// In-game (non-paused, non-menu etc.)
	{
		if (devConsole.isOpen())
		{
			m_player.lock();
			m_camera.lock();
		}
		else
		{
			m_player.unlock();
			m_camera.unlock();
		}

		updateAI(fstep);														// update AI/scripts etc. (TBI)
		updateWorld(fstep);														// update moving objects, triggers etc. (TBI)
		updatePlayerControls(&m_keys, &m_controller[0], fstep);					// update player inputs
		updatePhysics(fstep);													// btWorld step
		updateCameraControls(&m_mouse, &m_keys, &m_controller[0], fstep);		// update camera (--> mat_view)
	}
	else if (ifGameState(GAMESTATE_PAUSED) && !devConsole.isOpen())	// Game is paused
	{
		m_camera.unlock();

		updateCameraControls(&m_mouse, &m_keys, &m_controller[0], fstep);		// update camera (--> mat_view)
	}

	prepareFrame();														// prepare the frame for rendering
}
void Antimony::endStep()
{
	devConsole.draw(&display, getDelta(), &Consolas);					// Draw the dev console

	if (game.debug)
	{
		render_Debug();													// render debug info
	}
	presentFrame();														// present frame to the GPU

	if (game.debug)
		monitorLog();													// log some debugging info to the debug monitor
	m_mouse.reset();
	m_keys.reset();
	for (unsigned char i = 0; i< XUSER_MAX_COUNT; i++)
	{
		if (m_controller[i].isEnabled())
			m_controller[i].reset();
	}
}
HRESULT Antimony::prepareFrame()
{
	devcon->ClearRenderTargetView(targettview, RGBA{ 0.0f, 0.2f, 0.4f, 0.0f });					// clear the render target view
	devcon->ClearDepthStencilView(depthstencilview, D3D11_CLEAR_DEPTH, 1.0f, 0);				// clear depth stencil

	//devcon->IASetVertexBuffers(0, 1, &vertexbuffer, (UINT*)sizeof(VERTEX_BASIC), (UINT*)(0));	// set vertex buffer
	devcon->IASetIndexBuffer(indexbuffer, DXGI_FORMAT_R32_UINT, 0);								// set index buffer
	devcon->VSSetConstantBuffers(0, 1, &constantbuffer);										// set constant buffer

	Consolas.clearFW1();
	Arial.clearFW1();

	return S_OK;
}
HRESULT Antimony::presentFrame()
{
	Consolas.presentFW1();
	Arial.presentFW1();

	if (display.vsync)
		return swapchain->Present(1, 0);
	else
		return swapchain->Present(0, DXGI_PRESENT_DO_NOT_WAIT);
	frame_count++;
}
void Antimony::updatePlayerControls(KeysController *khandle, XInputController *xhandle, double delta)
{
	float speed = 4 * m_player.m_movSpeed;
	if (khandle->sprint.getState() > BTN_UNPRESSED || xhandle->B.getState() > BTN_UNPRESSED)
		speed = 8 * m_player.m_movSpeed;

	float3 mmov, xmov;
	float th = m_camera.getAngle(CAM_THETA);

	if (m_player.isFree())
	{
		if (khandle->forward.getState() > BTN_UNPRESSED)
			mmov += float3(cosf(th), 0, sinf(th));
		if (khandle->backward.getState() > BTN_UNPRESSED)
			mmov += float3(-cosf(th), 0, -sinf(th));
		if (khandle->left.getState() > BTN_UNPRESSED)
			mmov += float3(-sinf(th), 0, cosf(th));
		if (khandle->right.getState() > BTN_UNPRESSED)
			mmov += float3(sinf(th), 0, -cosf(th));
	}

	xmov = float3(sinf(th) * xhandle->LX.getVel() + cosf(th) * xhandle->LY.getVel(), 0, sinf(th) * xhandle->LY.getVel() + cosf(th) * (-xhandle->LX.getVel()));

	float3 mov = mmov + xmov;
	if (mov.Length() > 1)
		mov = XMVector3Normalize(mov);

	//player.MoveToPoint(player.GetPosDest() + mov * speed * delta, .999999971);

	m_player.update(delta, m_objectsCollisions);
	m_player.move(&Float3Tobt(&mov), speed);

	if (m_player.isFree())
	{
		if (khandle->jump.getState() > BTN_UNPRESSED || xhandle->A.getState() > BTN_UNPRESSED)
			m_player.attemptJump();
	}
}
void Antimony::updateCameraControls(MouseController *mhandle, KeysController *khandle, XInputController *xhandle, double delta)
{
	float3 eye = v3_origin;
	float m_slide = 0.005 * controls.m_sensitivity;
	float x_slide = 6 * controls.x_sensitivity * delta;
	float radius = 1;
	float maxpitch = 0.1;
	//static float zoom = 1;
	static float _theta = MATH_PI / 2;
	static float _phi = MATH_PI / 2;

	// camera rotation
	if (m_camera.isFree())
	{
		if (mhandle->RMB.getState() == BTN_UNPRESSED && xhandle->LT.getState() < BTN_PRESSED)
		{
			_theta -= m_slide * mhandle->X.getVel() * BoolToSign(controls.m_invertxaxis)
				+ x_slide * xhandle->RX.getVel() * BoolToSign(controls.x_invertxaxis);
			_phi += m_slide * mhandle->Y.getVel() * BoolToSign(controls.m_invertyaxis)
				+ x_slide * xhandle->RY.getVel() * -BoolToSign(controls.x_invertyaxis);
		}
		else if (mhandle->RMB.getState() > BTN_UNPRESSED || xhandle->LT.getState() > BTN_UNPRESSED) // zoom
		{
			m_camera.displacement += float(mhandle->Y.getVel()) * 0.0015;
			m_camera.displacement -= float(xhandle->RY.getVel()) * 0.0015;
		}
		m_camera.zoom -= float(mhandle->Z.getVel()) * 0.0015;
	}

	if (m_camera.displacement < 0.2)
		m_camera.displacement = 0.2;
	if (m_camera.zoom < 0.1)
		m_camera.zoom = 0.1;
	if (m_camera.zoom > 3.9)
		m_camera.zoom = 3.9;
	if (_phi >= MATH_PI - maxpitch)
		_phi = MATH_PI - maxpitch;
	if (_phi <= maxpitch)
		_phi = maxpitch;

	eye.x = (radius + m_camera.displacement * m_camera.displacement) * cosf(_theta) * sinf(_phi);
	eye.y = (radius + m_camera.displacement * m_camera.displacement) * cosf(_phi);
	eye.z = (radius + m_camera.displacement * m_camera.displacement) * sinf(_theta) * sinf(_phi);

	float3 height = float3(0, 0.26, 0);
	float3 entity = MatToFloat3(&m_physEntities.at(game.dbg_entityfollow%m_physEntities.size())->getMatTransform());//player.getPos();

	m_camera.lookAtPoint(entity + WORLD_SCALE * (height + eye), game.camera_friction * (.99999999) + !game.camera_friction);
	m_camera.moveToPoint(entity + WORLD_SCALE * (height - eye * m_camera.displacement), game.camera_friction * (.9999999) + !game.camera_friction);

	// reset camera
	if (mhandle->MMB.getState() == BTN_HELD || xhandle->RS.getState() == BTN_HELD)
	{
		m_camera.lock();
		//camera.LookAtPoint(v3_origin, .99999);
		m_camera.lookAtPoint(m_player.getColl()->getFloat3Pos(), .99999);
		m_camera.displacement = 1;
		m_camera.zoom = 1;
	}
	else if (!m_camera.isFree())
		m_camera.unlock();

	m_camera.update(delta);

	float3 look = entity + WORLD_SCALE * height;
	float3 pos = m_camera.getPos();
	float3 ray = (pos - look); ray.Normalize();
	float3 start = look + 0.1 * ray;

	float3 finalpos = pos;

	if (m_camera.displacement > 0.1 && true)
	{
		btVector3 btFrom = Float3Tobt(&start);
		btVector3 btTo = Float3Tobt(&pos);
		btCollisionWorld::ClosestRayResultCallback res(btFrom, btTo);
		m_btWorld->rayTest(btFrom, btTo, res);

		//btTransform t; t.setIdentity();
		//t.setOrigin(Float3Tobt(&start));
		if (res.hasHit()) {
			finalpos = btToFloat3(&res.m_hitPointWorld) - 0.1 * ray;
			//t.setOrigin(res.m_hitPointWorld);
		}
		//m_camera.getColl()->getRigidBody()->setWorldTransform(t);
	}

	mat_view = MLookAtLH(finalpos, m_camera.getLookAt(), float3(0, 1, 0));
	mat_proj = MPerspFovLH(m_camera.zoom * MATH_PI / 4, window_main.aspect, 0.001f, 10000.0f);
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
		h += 0.5f * MATH_PI * delta;
	if (h >= 2 * MATH_PI)
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
	switch (getGameState())
	{
		case GAMESTATE_INGAME:				// In-game (non-paused, non-menu etc.)
		{
			if (m_keys.sk_escape.getState() == BTN_PRESSED && !devConsole.isOpen())		// Escape
			{
				if (game.debug)
					PostQuitMessage(0);
				else
					setGameState(GAMESTATE_PAUSEMENU);
			}
			else if (m_keys.pause.getState() == BTN_PRESSED && !devConsole.isOpen())		// Pause key
			{
				setGameState(GAMESTATE_PAUSED);
			}
			break;
		}
		case GAMESTATE_PAUSEMENU:			// Pause menu (in-game inventory/pause menu)
		{
			if (m_keys.sk_escape.getState() == BTN_PRESSED && !devConsole.isOpen())
			{
				// TODO: Implement quit button & pause menu
				//setGameState(GAMESTATE_INGAME);
				PostQuitMessage(0);
			}
			break;
		}
		case GAMESTATE_PAUSED:
		{
			if (m_keys.sk_escape.getState() == BTN_PRESSED && !devConsole.isOpen())		// Escape
			{
				if (game.debug)
					PostQuitMessage(0);
				else
					setGameState(GAMESTATE_PAUSEMENU);
			}
			else if (m_keys.pause.getState() == BTN_PRESSED && !devConsole.isOpen())		// Pause key
			{
				setGameState(GAMESTATE_INGAME);
			}
			break;
		}
	}

	if (m_keys.console.getState() == BTN_PRESSED)		// Dev console
	{
		if (devConsole.isEnabled())
		{
			if (devConsole.isOpen())
				devConsole.close();
			else
				devConsole.open();
		}
	}
}
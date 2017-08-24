#include "Step.h"
#include "Renderer.h"
#include "Timer.h"
#include "Console.h"
#include "Debug.h"
#include "Gameflow.h"
#include "Gameplay.h"
#include "CpuUsage.h"

///

char BoolToSign(bool b)
{
	if (b == false)
		return 1;
	if (b == true)
		return -1;
	return 0;
}

///

void Antimony::step()
{
	timer.catchTime(TIMER_FRAME_GLOBAL);
	delta = timer.getDelta(TIMER_FRAME_GLOBAL);
	double fstep = delta * worldSpeed;

	cpuUsage.GetUsage(0);

	if (GetFocus() == window_main.hWnd)
	{
		ShowCursor(false);
		ClipCursor(&window_main.plane);
	}
	/*static double program_life = 0;
	life += delta;

	if (life > 10)
	{
		if (GetFocus() == window_main.hWnd)
		{
			ShowCursor(false);
			ClipCursor(&window_main.plane);
		}
		life = 0;
	}*/

	updateGameState();

	if (ifGameState(GAMESTATE_INGAME))									// In-game (non-paused, non-menu etc.)
	{
		if (devConsole.isOpen())
		{
			player.lock();
			camera_main.lock();
		}
		else
		{
			player.unlock();
			camera_main.unlock();
		}

		updateAI(fstep);														// update AI/scripts etc. (TBI)
		updateWorld(fstep);														// update moving objects, triggers etc. (TBI)
		updatePlayerControls(&keys, &controller[0], fstep);						// update player inputs
		updatePhysics(fstep);													// btWorld step
		updateCameraControls(&mouse, &keys, &controller[0], fstep);				// update camera (--> mat_view)
	}
	else if (ifGameState(GAMESTATE_PAUSED) && !devConsole.isOpen())		// Game is paused
	{
		camera_main.unlock();

		updatePlayerControls(&keys, &controller[0], fstep);						// update player inputs
		updateCameraControls(&mouse, &keys, &controller[0], fstep);				// update camera (--> mat_view)
	}

	prepareFrame();														// prepare the frame for rendering
}
void Antimony::endStep()
{
	devConsole.draw(&display, getDelta(), &Consolas);					// Draw the dev console
	if (game.debug)
		render_Debug();													// render debug info
	Antimony::timer.catchTime(TIMER_RENDER_DEBUG);

	presentFrame();														// present frame to the GPU

	if (game.debug)
		monitorLog();													// log some debugging info to the debug monitor
	mouse.reset();
	keys.reset();
	for (unsigned char i = 0; i< XUSER_MAX_COUNT; i++)
	{
		if (controller[i].isEnabled())
			controller[i].reset();
	}
	timer.catchTime(TIMER_AFTERSTEP);
}
HRESULT Antimony::prepareFrame()
{
	devcon->ClearRenderTargetView(targettview, RGBA{ 0.0f, 0.2f, 0.4f, 0.0f });					// clear the render target view
	devcon->ClearDepthStencilView(depthstencilview, D3D11_CLEAR_DEPTH, 1.0f, 0);				// clear depth stencil

	//devcon->IASetVertexBuffers(0, 1, &vertexbuffer, (UINT*)sizeof(VERTEX_BASIC), (UINT*)(0));	// set vertex buffer
	devcon->IASetIndexBuffer(indexbuffer, DXGI_FORMAT_R32_UINT, 0);								// set index buffer
	devcon->VSSetConstantBuffers(0, 1, &constantbuffer);										// set constant buffer

	//ID3D11RasterizerState *rss;	devcon->RSGetState(&rss); D3D11_RASTERIZER_DESC rzd; rss->GetDesc(&rzd);

	Consolas.clearFW1();
	Arial.clearFW1();

	if (Antimony::game.dbg_wireframe)
		devcon->RSSetState(rss_wireframe);
	else
		devcon->RSSetState(rss_standard);

	timer.catchTime(TIMER_PRESTEP);

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
	float speed = 4 * player.m_movSpeed;
	if (khandle->sprint.getState() > BTN_UNPRESSED || xhandle->B.getState() > BTN_UNPRESSED)
		speed = 8 * player.m_movSpeed;

	float3 mmov, xmov;
	float th = camera_main.getAngle(CAM_THETA);
	float ph = camera_main.getAngle(CAM_PHI);

	float3 mov;

	if (camera_main.object == player.getColl())
	{
		if (khandle->forward.getState() > BTN_UNPRESSED)
			mmov += float3(cosf(th), 0, sinf(th));
		if (khandle->backward.getState() > BTN_UNPRESSED)
			mmov += float3(-cosf(th), 0, -sinf(th));
		if (khandle->left.getState() > BTN_UNPRESSED)
			mmov += float3(-sinf(th), 0, cosf(th));
		if (khandle->right.getState() > BTN_UNPRESSED)
			mmov += float3(sinf(th), 0, -cosf(th));

		xmov = float3(sinf(th) * xhandle->LX.getVel() + cosf(th) * xhandle->LY.getVel(), 0, sinf(th) * xhandle->LY.getVel() + cosf(th) * -xhandle->LX.getVel());

		mov = mmov + xmov;
		if (mov.Length() > 1)
			mov = XMVector3Normalize(mov);
	}
	else
	{
		float3 dir = camera_main.getLookAt() - camera_main.getPos();

		if (khandle->forward.getState() > BTN_UNPRESSED)
		{
			//float3(cosf(th) * sinf(ph), -ph / (camera_main.maxPitch - camera_main.minPitch) * 2 + 1 + camera_main.minPitch, sinf(th) * sinf(ph));
			mmov += dir;
			mmov = XMVector3Normalize(mmov);
		}

		if (khandle->backward.getState() > BTN_UNPRESSED)
		{
			//float3(-cosf(th) * sinf(ph), ph / (camera_main.maxPitch - camera_main.minPitch) * 2 - 1 - camera_main.minPitch, -sinf(th) * sinf(ph));
			mmov -= dir;
			mmov = XMVector3Normalize(mmov);
		}
		if (khandle->left.getState() > BTN_UNPRESSED)
			mmov += float3(-sinf(th), 0, cosf(th));
		if (khandle->right.getState() > BTN_UNPRESSED)
			mmov += float3(sinf(th), 0, -cosf(th));

		xmov = float3(dir.x * xhandle->LX.getVel() + cosf(th) * xhandle->LY.getVel(),
			dir.y * xhandle->LX.getVel(),
			sinf(th) * xhandle->LY.getVel() + dir.z * -xhandle->LX.getVel());

		mov = mmov + xmov;
	}

	//player.MoveToPoint(player.GetPosDest() + mov * speed * delta, .999999971);

	player.update(delta, m_objectsCollisions);

	if (player.isFree() && camera_main.object == player.getColl())
	{
		player.move(&Float3Tobt(&mov), speed);
		if (khandle->jump.getState() > BTN_UNPRESSED || xhandle->A.getState() > BTN_UNPRESSED)
			player.attemptJump();
	}
	else if (camera_main.isFree())
	{
		player.move(&Float3Tobt(&v3_origin), speed); // correct player position
		camera_main.displacement += mov * speed * camera_main.dolly * delta; // move free flyby camera
	}
}
void Antimony::updateCameraControls(MouseController *mhandle, KeysController *khandle, XInputController *xhandle, double delta)
{
	float3 eye = v3_origin;
	float m_slide = 0.005 * controls.m_sensitivity;
	float x_slide = 6 * controls.x_sensitivity * delta;
	float radius = 1;
	static float _theta = MATH_PI / 2;
	static float _phi = MATH_PI / 2;

	// camera rotation
	if (camera_main.isFree())
	{
		if (mhandle->RMB.getState() == BTN_UNPRESSED && xhandle->LT.getState() < BTN_PRESSED) // look
		{
			_theta -= m_slide * mhandle->X.getVel() * BoolToSign(controls.m_invertxaxis)
				+ x_slide * xhandle->RX.getVel() * BoolToSign(controls.x_invertxaxis);
			_phi += m_slide * mhandle->Y.getVel() * BoolToSign(controls.m_invertyaxis)
				+ x_slide * xhandle->RY.getVel() * -BoolToSign(controls.x_invertyaxis);
		}
		if (camera_main.isDollyEnabled()) // dolly
		{
			if (camera_main.object == nullptr)
				camera_main.dolly -= float(mhandle->Z.getVel()) * 0.0015;
			if (mhandle->RMB.getState() > BTN_UNPRESSED || xhandle->LT.getState() > BTN_UNPRESSED)
			{
				camera_main.dolly += float(mhandle->Y.getVel()) * 0.0015;
				camera_main.dolly -= float(xhandle->RY.getVel()) * 0.0015;
			}
		}
		if (camera_main.isZoomEnabled() && camera_main.object != nullptr)
			camera_main.zoom -= float(mhandle->Z.getVel()) * 0.0015; // zoom
	}

	if (camera_main.dolly < camera_main.minDolly) // 0.2
		camera_main.dolly = camera_main.minDolly;
	if (camera_main.dolly > camera_main.maxDolly && camera_main.maxDolly != -1) // -1
		camera_main.dolly = camera_main.maxDolly;
	if (camera_main.zoom < camera_main.minZoom) // 0.1
		camera_main.zoom = camera_main.minZoom;
	if (camera_main.zoom > camera_main.maxZoom && camera_main.maxZoom != -1) // 3.9
		camera_main.zoom = camera_main.maxZoom;
	if (_phi >= camera_main.maxPitch)
		_phi = camera_main.maxPitch;
	if (_phi <= camera_main.minPitch)
		_phi = camera_main.minPitch;

	eye.x = (radius + camera_main.dolly * camera_main.dolly) * cosf(_theta) * sinf(_phi);
	eye.y = (radius + camera_main.dolly * camera_main.dolly) * cosf(_phi);
	eye.z = (radius + camera_main.dolly * camera_main.dolly) * sinf(_theta) * sinf(_phi);

	float3 height = camera_main.displacement; //float3(0, 0.26, 0);
	float3 entity = v3_origin;
	if (camera_main.object)
		entity = camera_main.object->getFloat3Pos(); //MatToFloat3(&physEntities.at(game.dbg_entityfollow%physEntities.size())->getMatTransform());//player.getPos();

	camera_main.lookAtPoint(entity + WORLD_SCALE * (height + eye), game.camera_friction * (.99999999) + !game.camera_friction);
	camera_main.moveToPoint(entity + WORLD_SCALE * (height - eye * camera_main.dolly), game.camera_friction * (.9999999) + !game.camera_friction);

	// reset camera
	if (mhandle->MMB.getState() == BTN_HELD || xhandle->RS.getState() == BTN_HELD)
	{
		camera_main.lock();
		//camera.LookAtPoint(v3_origin, .99999);
		camera_main.lookAtPoint(player.getColl()->getFloat3Pos(), .99999);
		camera_main.dolly = 1;
		camera_main.zoom = 1;
	}
	else if (!camera_main.isFree())
		camera_main.unlock();

	camera_main.update(delta);

	float3 look = entity + WORLD_SCALE * height;
	float3 pos = camera_main.getPos();
	float3 ray = (pos - look); ray.Normalize();
	float3 start = look + 0.1 * ray;

	float3 finalpos = pos;

	if (camera_main.dolly > 0.1 && !camera_main.isNoclip())
	{
		btVector3 btFrom = Float3Tobt(&start);
		btVector3 btTo = Float3Tobt(&pos);
		btCollisionWorld::ClosestRayResultCallback res(btFrom, btTo);
		btWorld->rayTest(btFrom, btTo, res);

		//btTransform t; t.setIdentity();
		//t.setOrigin(Float3Tobt(&start));
		if (res.hasHit()) {
			finalpos = btToFloat3(&res.m_hitPointWorld) - 0.1 * ray;
			//t.setOrigin(res.m_hitPointWorld);
		}
		//camera_main.getColl()->getRigidBody()->setWorldTransform(t);
	}

	mat_view = MLookAtLH(finalpos, camera_main.getLookAt(), float3(0, 1, 0));
	mat_proj = MPerspFovLH(camera_main.zoom * MATH_PI / 4, window_main.aspect, 0.001f, 10000.0f);
}
void Antimony::updateAI(double delta)
{
	// TODO: Implement AI
}
void Antimony::updatePhysics(double delta)
{
	// TODO: Implement physics

	btWorld->stepSimulation(delta, 10, 1.f / 240.f);
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

	physEntities.at(3)->setMatTransform(&(MTranslation(WORLD_SCALE * 0, WORLD_SCALE * 0.5, WORLD_SCALE * 0) * MRotY(h)));
	physEntities.at(4)->setMatTransform(&(MTranslation(WORLD_SCALE * 3, WORLD_SCALE * 1, WORLD_SCALE * sinf(h))));
	physEntities.at(4)->updateKinematic(delta);
	physEntities.at(5)->setMatTransform(&(MTranslation(WORLD_SCALE * 3, WORLD_SCALE * (1 - 0.5 * sinf(h)), 2)));
	//physEntities.at(5)->updateKinematic(delta);
	//physEntities.at(5)->getRigidBody()->setLinearVelocity(btVector3(0, WORLD_SCALE * (sinf(h)), 0));
	//physEntities.at(5)->getRigidBody()->setLinearFactor(btVector3(0, WORLD_SCALE * (sinf(h)), 0));
	//physEntities.at(5)->getRigidBody()->setLinearVelocity(bt_origin);
}
void Antimony::updateGameState()
{
	switch (getGameState())
	{
		case GAMESTATE_INGAME:				// In-game (non-paused, non-menu etc.)
		{
			if (keys.sk_escape.getState() == BTN_PRESSED && !devConsole.isOpen())		// Escape
			{
				if (game.debug)
					PostQuitMessage(0);
				else
					setGameState(GAMESTATE_PAUSEMENU);
			}
			else if (keys.pause.getState() == BTN_PRESSED && !devConsole.isOpen())		// Pause key
			{
				setGameState(GAMESTATE_PAUSED);
			}
			break;
		}
		case GAMESTATE_PAUSEMENU:			// Pause menu (in-game inventory/pause menu)
		{
			if (keys.sk_escape.getState() == BTN_PRESSED && !devConsole.isOpen())
			{
				// TODO: Implement quit button & pause menu
				//setGameState(GAMESTATE_INGAME);
				PostQuitMessage(0);
			}
			break;
		}
		case GAMESTATE_PAUSED:
		{
			if (keys.sk_escape.getState() == BTN_PRESSED && !devConsole.isOpen())		// Escape
			{
				if (game.debug)
					PostQuitMessage(0);
				else
					setGameState(GAMESTATE_PAUSEMENU);
			}
			else if (keys.pause.getState() == BTN_PRESSED && !devConsole.isOpen())		// Pause key
			{
				setGameState(GAMESTATE_INGAME);
			}
			break;
		}
	}

	if (keys.console.getState() == BTN_PRESSED)		// Dev console
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

double Antimony::getTick()
{
	return delta * worldSpeed;
}
double Antimony::getDelta()
{
	return delta;
}

namespace Antimony
{
	double delta = 0;
	float worldSpeed = 1;
}
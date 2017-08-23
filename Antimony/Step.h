#pragma once

#include <Windows.h>	// required for HRESULT

#include "Input.h"

///

namespace Antimony
{
	extern double delta;
	extern float worldSpeed;

	void step();
	void endStep();
	HRESULT prepareFrame();
	HRESULT presentFrame();
	void updatePlayerControls(KeysController *khandle, XInputController *xhandle, double delta);
	void updateCameraControls(MouseController *mhandle, KeysController *khandle, XInputController *xhandle, double delta);
	void updateAI(double delta);
	void updatePhysics(double delta);
	void updateWorld(double delta);
	void updateGameState();

	double getTick();
	double getDelta();
}
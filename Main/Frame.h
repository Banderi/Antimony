#ifndef FRAME_H
#define FRAME_H

#include <Windows.h>

#include "Geometry.h"
#include "Camera.h"
#include "Player.h"
#include "Input.h"

extern HRESULT hr;



//

class Timer
{
	float m_previousTime;
	float m_currentTime;
	float m_delta;
	float m_correction;

public:
	float GetDelta();
	void SetCorrection(float c);

	Timer();
};
extern Timer timer;

//

HRESULT Frame(double delta);

HRESULT PrepareFrame();
HRESULT PresentFrame();

void UpdatePlayerControls(KeysController *khandle, XInputController *xhandle, double delta);
void UpdateCameraControls(MouseController *mhandle, KeysController *khandle, XInputController *xhandle, double delta);

void UpdateHUD(double delta);
void UpdateAI(double delta);
void UpdatePhysics(double delta);
void UpdateWorld(double delta);
void Update_Debug(double delta);

void RenderWorld();
void RenderEntities();
void RenderHUD();
void Render_Debug();
void Render_DebugKeyboard(float3 pos);
void Render_DebugMouse(float3 pos);
void Render_DebugController(float3 pos, unsigned char c);

void SetDepthBufferState(bool state);

color BtnStateColor(Input bt);

#define ON true
#define OFF false

#endif
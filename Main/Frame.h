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
	double m_previousClock[32];
	double m_currentClock[32];
	double m_delta[32];	
	double m_correction;
	double m_clockStamp;
	int m_framesCount;
	float m_fpsStamp;

public:
	void UpdateDelta(char d);
	void SetCorrection(float c);
	double GetDelta(char d);
	int GetFramesCount();
	float GetFPSStamp();

	Timer();
};
extern Timer timer;

#define TIMER_FRAME_GLOBAL	0x00000000
#define TIMER_TEST			0x00000001

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

void RenderWorld();
void RenderEntities();
void RenderHUD();
void Render_Debug(double delta);
void Render_DebugKeyboard(float2 pos);
void Render_DebugMouse(float2 pos);
void Render_DebugController(float2 pos, unsigned char c);

void SetDepthBufferState(bool state);

color BtnStateColor(Input bt);

#define ON true
#define OFF false

#endif
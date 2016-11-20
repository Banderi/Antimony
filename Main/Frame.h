#pragma once

#include <Windows.h>

#include "Geometry.h"
#include "Camera.h"
#include "Player.h"
#include "Input.h"

//

extern wchar_t global_str64[64];

//

HRESULT Frame(double delta);

HRESULT PrepareFrame();
HRESULT PresentFrame();

void UpdatePlayerControls(KeysController *khandle, XInputController *xhandle, double delta);
void UpdateCameraControls(MouseController *mhandle, KeysController *khandle, XInputController *xhandle, double delta);

void UpdateAI(double delta);
void UpdatePhysics(double delta);
void UpdateWorld(double delta);

void RenderWorld();
void RenderEntities();
void RenderHUD(double delta);
void Render_Debug(double delta);
void Render_DebugKeyboard(float2 pos);
void Render_DebugMouse(float2 pos);
void Render_DebugController(float2 pos, unsigned char c);
void Render_DebugFPS(float2 pos);

void SetDepthBufferState(bool state);

color BtnStateColor(Input bt);

#define ON true
#define OFF false
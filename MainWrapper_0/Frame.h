#ifndef FRAME_H
#define FRAME_H

#include <Windows.h>

#include "Geometry.h"
#include "Camera.h"
#include "Player.h"
#include "Input.h"

extern HRESULT hr;

extern IDXGISwapChain *swapchain;
extern ID3D11Device *dev;
extern ID3D11DeviceContext *devcon;

extern ID3D11RenderTargetView *targettview;
extern ID3D11DepthStencilView *depthstencilview;

extern ID3D11RasterizerState *rasterizerstate;
extern ID3D11DepthStencilState *depthstencil_enabled;
extern ID3D11DepthStencilState *depthstencil_disabled;

extern ID3D11VertexShader *pVShader, *pCelVS, *pOutlineVS;
extern ID3D11PixelShader *pPShader, *pCelPS, *pOutlinePS;
extern ID3D11InputLayout *pLayout, *pCelLayout, *pOutlineLayout;

extern ID3D11Buffer *pVertexBuffer;
extern ID3D11Buffer *pIndexBuffer;
extern ID3D11Buffer *pConstantBuffer;

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

HRESULT RenderFrameDX9(float delta);		// <-- yah, I'm serious
HRESULT RenderFrame(float delta);

HRESULT PrepareFrame();
HRESULT PresentFrame();

HRESULT SetView(mat *world, mat *view, mat *proj);
void UpdatePlayerControls(Keys* khandle, float delta);
void UpdateCameraControls(Mouse* mhandle, float delta);

void UpdateHUD(float delta);
void UpdateAI(float delta);
void UpdatePhysics(float delta);
void UpdateWorld(float delta);

void Update_DebugCube(float delta);

void SetDepthBufferState(bool state);

#define ON true
#define OFF false

#endif
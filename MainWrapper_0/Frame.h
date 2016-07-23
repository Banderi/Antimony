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
HRESULT RenderFrameDX11(float delta);

HRESULT PrepareFrame();
HRESULT PresentFrame();

void PlayerControls(Keys* khandle, float delta);
void CameraControls(Mouse* mhandle, float delta);
HRESULT SetView(mat *world, mat *view, mat *proj);

void SetDepthBufferState(bool state);

#define ON true
#define OFF false

#endif
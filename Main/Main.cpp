#include "Warnings.h"

#include <string>
#include <Shlwapi.h>

#include "Main.h"
#include "Window.h"
#include "Frame.h"
#include "DebugWin.h"
#include "Hresult.h"
#include "Controls.h"
#include "Gameflow.h"

using namespace std;

double delta = 0;
float worldSpeed = 1;

int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	SetGameState(GAMESTATE_LOADING_1);

	ReadConfig();
		#ifdef _DEBUG
	InitializeDebugConsole();
	WriteToConsole(L"---> START OF DEBUG CONSOLE <---\n");
		#endif	

	CreateMainWindow(hInstance);
	ShowWindow(hWnd, nCmdShow);

	if (!Handle(&hr, HRH_MAIN_ENUMHW, EnumHardware()))
		return 0;

	if (!Handle(&hr, HRH_MAIN_INITD3D, InitD3D(hWnd)))
		return 0;

	if (!Handle(&hr, HRH_MAIN_REGHID, InitControls()))
		return 0;

	if (!Handle(&hr, HRH_MAIN_INITSHADERS, InitShaders()))
		return 0;

	if (!Handle(&hr, HRH_MAIN_INITGRAPHICS, InitGraphics()))
		return 0;

	if (!Handle(&hr, HRH_MAIN_STARTINGFILES, LoadStartingFiles()))
		return 0;

	ShowCursor(false);
	ClipCursor(&wScreen);

	//

	MSG msg;
	bool run = 1;

	player.moveToPoint(float3(0, 0, -3), -1);

	WriteToConsole(L"Entering main loop...\n");
	camera.unlock();

	SetGameState(GAMESTATE_INGAME);

	while (run)
	{
		while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
		{
			switch (msg.message)
			{
				case WM_QUIT:
				{
					run = 0;
					break;
				}
				case WM_INPUT:
				{
					HandleRaw(msg);
					break;
				}
				default:
				{
					TranslateMessage(&msg);
					DispatchMessageW(&msg);
					break;
				}
			}
		}

		delta = timer.GetDelta();
		Frame(delta * worldSpeed);

	#ifdef _DEBUG
		Log();
	#endif
		mouse.Reset();
		keys.Reset();
		for (unsigned char i = 0; i< XUSER_MAX_COUNT; i++)
		{
			if (controller[i].enabled)
				controller[i].Reset();
		}
	}
	WriteToConsole(L"Main loop terminated!\n");

	CleanD3D();

	return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
			break;
		} 
		case WM_KEYDOWN:
		{
			switch (wParam)
			{
				case VK_ESCAPE:
				{
					PostQuitMessage(0);
					break;
				}
			}
		}
	}
	return DefWindowProcW(hWnd, message, wParam, lParam);
}

void ReadConfig()
{
	WriteToConsole(L"Loading config files... ");

	wVSync = GetPrivateProfileIntW(L"display", L"VSync", 0, L".\\config.ini");
	wFullscreen = GetPrivateProfileIntW(L"display", L"Fullscreen", 0, L".\\config.ini");

	if (wFullscreen)
	{
		RECT desktop;
		const HWND hDesktop = GetDesktopWindow();
		GetWindowRect(hDesktop, &desktop);
		wWidth = desktop.right;
		wHeight = desktop.bottom;
		wX = 0;
		wY = 0;
		wAspectRatio = (float)wWidth / (float)wHeight;
	}
	else
	{
		wWidth = GetPrivateProfileIntW(L"display", L"WindowWidth", 800, L".\\config.ini");
		wHeight = GetPrivateProfileIntW(L"display", L"WindowHeight", 600, L".\\config.ini");
		wX = GetPrivateProfileIntW(L"display", L"WindowX", 0, L".\\config.ini");
		wY = GetPrivateProfileIntW(L"display", L"WindowY", 0, L".\\config.ini");
		wBorderless = GetPrivateProfileIntW(L"display", L"Borderless", 0, L".\\config.ini");
		wAspectRatio = (float)wWidth / (float)wHeight;
	}

	wScreen = { wX, wY, wWidth + wX, wHeight + wY};

	// Get controls settings
	WCHAR buf[32];
	GetPrivateProfileStringW(L"controls", L"mSensibility", L".5", buf, 32, L".\\config.ini");
	mSensibility = _wtof(buf);
	GetPrivateProfileStringW(L"controls", L"xSensibility", L".5", buf, 32, L".\\config.ini");
	xSensibility = _wtof(buf);

	mouseXAxis = GetPrivateProfileIntW(L"controls", L"InvertMouseXAxis", 0, L".\\config.ini");
	mouseYAxis = GetPrivateProfileIntW(L"controls", L"InvertMouseYAxis", 0, L".\\config.ini");
	controllerXAxis = GetPrivateProfileIntW(L"controls", L"InvertControllerXAxis", 0, L".\\config.ini");
	controllerYAxis = GetPrivateProfileIntW(L"controls", L"InvertControllerYAxis", 0, L".\\config.ini");

	GetPrivateProfileStringW(L"controls", L"kForward", L"0x57", buf, 32, L".\\config.ini");
	StrToIntExW(buf, STIF_SUPPORT_HEX, (int*)(&kForward));
	GetPrivateProfileStringW(L"controls", L"kBackward", L"0x53", buf, 32, L".\\config.ini");
	StrToIntExW(buf, STIF_SUPPORT_HEX, (int*)(&kBackward));
	GetPrivateProfileStringW(L"controls", L"kLeft", L"0x41", buf, 32, L".\\config.ini");
	StrToIntExW(buf, STIF_SUPPORT_HEX, (int*)(&kLeft));
	GetPrivateProfileStringW(L"controls", L"kRight", L"0x44", buf, 32, L".\\config.ini");
	StrToIntExW(buf, STIF_SUPPORT_HEX, (int*)(&kRight));

	GetPrivateProfileStringW(L"controls", L"kSprint", L"0xA0", buf, 32, L".\\config.ini");
	StrToIntExW(buf, STIF_SUPPORT_HEX, (int*)(&kSprint));
	GetPrivateProfileStringW(L"controls", L"kJump", L"0x20", buf, 32, L".\\config.ini");
	StrToIntExW(buf, STIF_SUPPORT_HEX, (int*)(&kJump));
	GetPrivateProfileStringW(L"controls", L"kAction", L"0x45", buf, 32, L".\\config.ini");
	StrToIntExW(buf, STIF_SUPPORT_HEX, (int*)(&kAction));

	WriteToConsole(L"done\n");
}
HRESULT EnumHardware()
{
	WriteToConsole(L"Enumerating hardware... ");

	HRESULT hr;
	IDXGIFactory* factory;
	IDXGIAdapter* adapter;
	IDXGIOutput* adapterOutput;
	unsigned int numModes, i, stringLength;
	DXGI_MODE_DESC* displayModeList;
	DXGI_ADAPTER_DESC adapterDesc;
	int error;
	/*DXGI_SWAP_CHAIN_DESC swapChainDesc;
	D3D_FEATURE_LEVEL featureLevel;
	ID3D11Texture2D* backBufferPtr;
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	D3D11_RASTERIZER_DESC rasterDesc;
	D3D11_VIEWPORT viewport;
	float fieldOfView, screenAspect;*/

	// Create a DirectX graphics interface factory.
	if (!Handle(&hr, HRH_ENUM_CREATEDXGIFACTORY, CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory)))
		return hr;

	// Use the factory to create an adapter for the primary graphics interface (video card).
	if (!Handle(&hr, HRH_ENUM_ENUMGPU, factory->EnumAdapters(0, &adapter)))
		return hr;

	// Enumerate the primary adapter output (monitor).
	if (!Handle(&hr, HRH_ENUM_ENUMOUTPUTDEVICE, adapter->EnumOutputs(0, &adapterOutput)))
		return hr;

	// Get the number of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format for the adapter output (monitor).
	if (!Handle(&hr, HRH_ENUM_GETOUTPUTMODESNUMBER, adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL)))
		return hr;

	// Create a list to hold all the possible display modes for this monitor/video card combination.
	displayModeList = new DXGI_MODE_DESC[numModes];

	// Now fill the display mode list structures.
	if (!Handle(&hr, HRH_ENUM_FILLOUTPUTMODESLIST, adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList)))
		return hr;

	RECT desktop;
	const HWND hDesktop = GetDesktopWindow();
	GetWindowRect(hDesktop, &desktop);

	// Now go through all the display modes and find the one that matches the screen width and height.
	// When a match is found store the numerator and denominator of the refresh rate for that monitor.
	for (i = 0; i<numModes; i++)
	{
		if (displayModeList[i].Width == (unsigned int)desktop.right)
		{
			if (displayModeList[i].Height == (unsigned int)desktop.bottom)
			{
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	// Get the adapter (video card) description.
	if (!Handle(&hr, HRH_ENUM_GETGPUDESC, adapter->GetDesc(&adapterDesc)))
		return hr;

	// Store the dedicated video card memory in megabytes.
	videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	// Convert the name of the video card to a character array and store it.
	error = wcstombs_s(&stringLength, videoCardDescription, 128, adapterDesc.Description, 128);
	if (error != 0)
		return error;

	// Release the display mode list.
	delete[] displayModeList;
	displayModeList = 0;

	// Release the adapter output, adapter and factory.
	smartRelease(adapterOutput);
	smartRelease(adapter);
	smartRelease(factory);

	WriteToConsole(L"done\n");

	return S_OK;
}
HRESULT InitD3D(HWND hWnd)
{
	WriteToConsole(L"Initializing D3D11... ");

	DXGI_SWAP_CHAIN_DESC scd;
	D3D11_TEXTURE2D_DESC txd;
	D3D11_DEPTH_STENCIL_DESC dsd;
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
	D3D11_RASTERIZER_DESC rzd;
	D3D11_VIEWPORT viewport;

	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));
	ZeroMemory(&txd, sizeof(D3D11_TEXTURE2D_DESC));
	ZeroMemory(&dsd, sizeof(D3D11_DEPTH_STENCIL_DESC));
	ZeroMemory(&dsvd, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	ZeroMemory(&rzd, sizeof(D3D11_RASTERIZER_DESC));
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	ID3D11Texture2D *pBackBufferTexture = NULL;
	ID3D11Texture2D *pDepthStencilBufferTexture = NULL;

	// Fill swapchain description
	scd.BufferCount = 1;
	scd.BufferDesc.Width = wWidth;
	scd.BufferDesc.Height = wHeight;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	if (wVSync)
	{
		scd.BufferDesc.RefreshRate.Numerator = numerator;
		scd.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else
	{
		scd.BufferDesc.RefreshRate.Numerator = 0;
		scd.BufferDesc.RefreshRate.Denominator = 1;
	}
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.OutputWindow = hWnd;
	scd.SampleDesc.Count = 1;
	scd.SampleDesc.Quality = 0;
	scd.Windowed = !wFullscreen;
	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	

	D3D_FEATURE_LEVEL featurelevel = D3D_FEATURE_LEVEL_11_0;

	// Create the swapchain
	if (!Handle(&hr, HRH_SWAPCHAIN_CREATE, D3D11CreateDeviceAndSwapChain(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
	#ifdef _DEBUG
		D3D11_CREATE_DEVICE_DEBUG,
	#else
		0,
	#endif
		&featurelevel,
		1,
		D3D11_SDK_VERSION,
		&scd,
		&swapchain,
		&dev,
		NULL,
		&devcon)))
		return hr;

	// Fill 2D texture description for the depth buffer
	txd.Width = wWidth;
	txd.Height = wHeight;
	txd.MipLevels = 1;
	txd.ArraySize = 1;
	txd.Format = DXGI_FORMAT_D32_FLOAT;
	txd.SampleDesc.Count = 1;
	txd.SampleDesc.Quality = 0;
	txd.Usage = D3D11_USAGE_DEFAULT;
	txd.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	txd.CPUAccessFlags = 0;
	txd.MiscFlags = 0;
	if (!Handle(&hr, HRH_DEPTHSTENCIL_TEXTURE, dev->CreateTexture2D(&txd, NULL, &pDepthStencilBufferTexture)))
		return hr;

	// Fill depth stencil description
	dsd.DepthEnable = true;
	dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsd.DepthFunc = D3D11_COMPARISON_LESS;
	if (!Handle(&hr, HRH_DEPTHSTENCIL_STATE, dev->CreateDepthStencilState(&dsd, &dss_enabled)))
		return hr;
	dsd.DepthEnable = false;
	if (!Handle(&hr, HRH_DEPTHSTENCIL_STATE, dev->CreateDepthStencilState(&dsd, &dss_disabled)))
		return hr;
	devcon->OMSetDepthStencilState(dss_enabled, 1);

	// Set up the depth stencil view description.
	dsvd.Format = txd.Format;
	dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvd.Texture2D.MipSlice = 0;
	if (!Handle(&hr, HRH_DEPTHSTENCIL_VIEW, dev->CreateDepthStencilView(pDepthStencilBufferTexture, &dsvd, &depthstencilview)))
		return hr;

	// Render target setup
	if (!Handle(&hr, HRH_SWAPCHAIN_SURFACEBUFFER, swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBufferTexture)))
		return hr;
	if (!Handle(&hr, HRH_RENDERTARGET_CREATE, dev->CreateRenderTargetView(pBackBufferTexture, NULL, &targettview))) // use the back buffer address to create the render target
		return hr;
	pBackBufferTexture->Release();
	pBackBufferTexture = NULL;
	devcon->OMSetRenderTargets(1, &targettview, depthstencilview);  // set the render target as the back buffer

	// Setup the raster description which will determine how and what polygons will be drawn.
	rzd.AntialiasedLineEnable = false;
	rzd.CullMode = D3D11_CULL_BACK;
	rzd.DepthBias = 0;
	rzd.DepthBiasClamp = 0.0f;
	rzd.DepthClipEnable = true;
	rzd.FillMode = D3D11_FILL_SOLID;
	rzd.FrontCounterClockwise = false;
	rzd.MultisampleEnable = false;
	rzd.ScissorEnable = false;
	rzd.SlopeScaledDepthBias = 0.0f;
	if (!Handle(&hr, HRH_RASTERIZER_STATE, dev->CreateRasterizerState(&rzd, &rasterizerstate)))
		return hr;

	// Viewport setup
	viewport.Width = wWidth;
	viewport.Height = wHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	devcon->RSSetViewports(1, &viewport);

	WriteToConsole(L"done\n");

	return S_OK;
}
HRESULT InitControls()
{
	hr = RegisterRID();
	if (FAILED(hr))
		return hr;

	keys.forward.Set(kForward, "W");
	keys.backward.Set(kBackward, "S");
	keys.left.Set(kLeft, "A");
	keys.right.Set(kRight, "D");

	keys.sprint.Set(kSprint, "Shift");
	keys.jump.Set(kJump, "Space");
	keys.action.Set(kAction, "E");

	return S_OK;
}
HRESULT InitShaders()
{
	WriteToConsole(L"Loading shaders... ");

	ID3D10Blob *blob = nullptr;

	// debug shaders
	if (!Handle(&hr, HRH_SHADER_COMPILE, D3DCompileFromFile(L".\\Shaders\\shader.hlsl", 0, 0, "VShader", "vs_4_0", 0, 0, &blob, 0)))
		return hr;
	if (!Handle(&hr, HRH_SHADER_CREATE, dev->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), NULL, &vs_debug)))
		return hr;

	if (!Handle(&hr, HRH_SHADER_INPUTLAYOUT, dev->CreateInputLayout(ied_debug, 2, blob->GetBufferPointer(), blob->GetBufferSize(), &il_debug)))
		return hr;

	if (!Handle(&hr, HRH_SHADER_COMPILE, D3DCompileFromFile(L".\\Shaders\\shader.hlsl", 0, 0, "PShader", "ps_4_0", 0, 0, &blob, 0)))
		return hr;
	if (!Handle(&hr, HRH_SHADER_CREATE, dev->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), NULL, &ps_debug)))
		return hr;

	WriteToConsole(L"done\n");
	return S_OK;
}
HRESULT InitGraphics()
{
	WriteToConsole(L"Loading starting graphics... ");

	D3D11_BUFFER_DESC bd; // buffer description
	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_BASIC) * 512;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	if (!Handle(&hr, HRH_GRAPHICS_VERTEXBUFFER, dev->CreateBuffer(&bd, NULL, &vertexbuffer)))
		return hr;

	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	if (!Handle(&hr, HRH_GRAPHICS_CONSTANTBUFFER, dev->CreateBuffer(&bd, NULL, &constantbuffer)))
		return hr;

	bd.ByteWidth = sizeof(UINT) * 512;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	if (!Handle(&hr, HRH_GRAPHICS_INDEXBUFFER, dev->CreateBuffer(&bd, NULL, &indexbuffer)))
		return hr;

	UINT stride = sizeof(VERTEX_BASIC);
	UINT offset = 0;
	devcon->IASetVertexBuffers(0, 1, &vertexbuffer, &stride, &offset);
	devcon->IASetIndexBuffer(indexbuffer, DXGI_FORMAT_R32_UINT, 0);
	devcon->VSSetConstantBuffers(0, 1, &constantbuffer);

	//

	mat_identity = MIdentity();
	mat_world = mat_identity;
	mat_view = mat_identity;
	mat_view = MLookAtLH(float3(0, 0, -1), float3(0, 0, 0), float3(0, 1, 0));
	mat_proj = MPerspFovLH(DX_PI / 4, wAspectRatio, 0.001f, 10000.0f);

	camera.moveToPoint(v_origin + float3(0, 0, -1), -1);
	camera.lookAtPoint(v_origin, -1);

	WriteToConsole(L"done\n");
	return S_OK;
}
HRESULT LoadStartingFiles()
{
	// TODO: Implement FBX & test some files against it

	return S_OK;
}
void CleanD3D()
{
	WriteToConsole(L"Cleaning D3D11... ");

	swapchain->SetFullscreenState(FALSE, NULL);
	swapchain->Release();
	dev->Release();
	devcon->Release();
	targettview->Release();
	depthstencilview->Release();

	WriteToConsole(L"done\n");

	ReleaseFiles();
}
void ReleaseFiles()
{
	WriteToConsole(L"Releasing files... ");

	smartRelease(vs_main);
	smartRelease(vs_debug);
	smartRelease(ps_main);
	smartRelease(ps_debug);

	WriteToConsole(L"done\n");
}

void Log()
{
	if (1)
	{
		static float c = 0.0f;
		c += delta;

		if (c >= 0.05)
		{
			/*WriteToConsole(L"\r                                    \r");
			WriteToConsole(L"Mouse position: ");
			WriteToConsole(to_wstring(mouse.GetCoord(xcoord).pos));
			WriteToConsole(L" (X) ");
			WriteToConsole(to_wstring(mouse.GetCoord(ycoord).pos));
			WriteToConsole(L" (X)");*/

			WriteToConsole(L"\r                                                            \r");
			WriteToConsole(to_wstring(controller[0].LX.vel));
			WriteToConsole(L" (LX) ");
			WriteToConsole(to_wstring(controller[0].LY.vel));
			WriteToConsole(L" (LY) ");
			WriteToConsole(to_wstring(controller[0].RX.vel));
			WriteToConsole(L" (RX) ");
			WriteToConsole(to_wstring(controller[0].RY.vel));
			WriteToConsole(L" (RY) ");
			
			//printf("\33[2K\r");
			//system("CLS");
			c = 0.0f;
		}		
	}
}
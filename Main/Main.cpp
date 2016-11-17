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
#include "SmartRelease.h"
#include "CpuRamUsage.h"
#include "Timer.h"
#include "Font.h"

using namespace std;

//

double delta = 0;
float worldSpeed = 1;

//

int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	SetGameState(GAMESTATE_LOADING_1);

	ReadConfig();

#ifdef _DEBUG
	game.debug = true;
#endif

	if (game.debug)
	{
		InitializeDebugConsole();
		WriteToConsole(L"---> START OF DEBUG CONSOLE <---\n");
	}

	CreateMainWindow(hInstance);
	ShowWindow(windowMain.hWnd, nCmdShow);

	if (!Handle(&hr, HRH_MAIN_ENUMHW, EnumHardware()))
		return 0;

	if (!Handle(&hr, HRH_MAIN_INITD3D, InitD3D(windowMain.hWnd)))
		return 0;

	if (!Handle(&hr, HRH_MAIN_REGHID, InitControls()))
		return 0;

	if (!Handle(&hr, HRH_MAIN_INITSHADERS, InitShaders()))
		return 0;

	if (!Handle(&hr, HRH_MAIN_INITFONTS, InitFonts()))
		return 0;

	if (!Handle(&hr, HRH_MAIN_INITGRAPHICS, InitGraphics()))
		return 0;

	if (!Handle(&hr, HRH_MAIN_STARTINGFILES, LoadStartingFiles()))
		return 0;

	ShowCursor(false);
	ClipCursor(&windowMain.plane);

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

		timer.UpdateDelta(TIMER_FRAME_GLOBAL);
		delta = timer.GetDelta(TIMER_FRAME_GLOBAL);
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

	// Get window settings
	windowMain.fullscreen = GetPrivateProfileIntW(L"display", L"Fullscreen", 0, L".\\config.ini");

	if (windowMain.fullscreen)
	{
		RECT desktop;
		const HWND hDesktop = GetDesktopWindow();
		GetWindowRect(hDesktop, &desktop);
		windowMain.width = desktop.right;
		windowMain.height = desktop.bottom;
		windowMain.X = 0;
		windowMain.Y = 0;
		windowMain.aspect = (float)windowMain.width / (float)windowMain.height;
	}
	else
	{
		windowMain.width = GetPrivateProfileIntW(L"display", L"WindowWidth", 800, L".\\config.ini");
		windowMain.height = GetPrivateProfileIntW(L"display", L"WindowHeight", 600, L".\\config.ini");
		windowMain.X = GetPrivateProfileIntW(L"display", L"WindowX", 0, L".\\config.ini");
		windowMain.Y = GetPrivateProfileIntW(L"display", L"WindowY", 0, L".\\config.ini");
		windowMain.borderless = GetPrivateProfileIntW(L"display", L"Borderless", 0, L".\\config.ini");
		windowMain.aspect = (float)windowMain.width / (float)windowMain.height;
	}

	windowMain.plane = { windowMain.X, windowMain.Y, windowMain.width + windowMain.X, windowMain.height + windowMain.Y};

	// Get display settings
	display.v_sync = GetPrivateProfileIntW(L"display", L"VSync", 0, L".\\config.ini");
	display.triple_buff = GetPrivateProfileIntW(L"display", L"TripleBuffering", 0, L".\\config.ini");

	// Get game settings
	game.debug = GetPrivateProfileIntW(L"game", L"Debug", 0, L".\\config.ini");
	game.cheats = GetPrivateProfileIntW(L"game", L"Cheats", 1, L".\\config.ini");
	game.difficulty = GetPrivateProfileIntW(L"game", L"Difficulty", 1, L".\\config.ini");
	game.camera_friction = GetPrivateProfileIntW(L"game", L"CameraWobble", 1, L".\\config.ini");

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

	cpu_usage.SetMaxRecords(100);

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
	// When a match is found store thedisplay.gpu_num and display.gpu_denom of the refresh rate for that monitor.
	for (i = 0; i<numModes; i++)
	{
		if (displayModeList[i].Width == (unsigned int)desktop.right)
		{
			if (displayModeList[i].Height == (unsigned int)desktop.bottom)
			{
				display.gpu_num = displayModeList[i].RefreshRate.Numerator;			WriteToConsole(to_wstring(display.gpu_num) + L" ");
				display.gpu_denom = displayModeList[i].RefreshRate.Denominator;		WriteToConsole(to_wstring(display.gpu_num) + L" ");
			}
		}
	}

	// Get the adapter (video card) description.
	if (!Handle(&hr, HRH_ENUM_GETGPUDESC, adapter->GetDesc(&adapterDesc)))
		return hr;

	// Store the dedicated video card memory in megabytes.
	display.gpu_vram = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);		WriteToConsole(to_wstring(display.gpu_vram) + L"mb ");

	// Convert the name of the video card to a character array and store it.
	error = wcstombs_s(&stringLength, display.gpu_desc, 128, adapterDesc.Description, 128);
	if (error != 0)
		return error;

	// Release the display mode list.
	delete[] displayModeList;
	displayModeList = 0;

	// Release the adapter output, adapter and factory.
	smartRelease(adapterOutput);
	smartRelease(adapter);
	smartRelease(factory);

	// Get RAM memory info
	memInfo.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&memInfo);
	if (!Handle(&hr, HRH_ENUM_GETMEMINFO, HRESULT_FROM_WIN32(GetLastError())))
		return hr;
	totalPhysMem = memInfo.ullTotalPhys;
	physMemAvail = memInfo.ullAvailPhys;

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
	D3D11_BLEND_DESC bsd;

	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));
	ZeroMemory(&txd, sizeof(D3D11_TEXTURE2D_DESC));
	ZeroMemory(&dsd, sizeof(D3D11_DEPTH_STENCIL_DESC));
	ZeroMemory(&dsvd, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	ZeroMemory(&rzd, sizeof(D3D11_RASTERIZER_DESC));
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
	ZeroMemory(&bsd, sizeof(D3D11_BLEND_DESC));

	ID3D11Texture2D *pBackBufferTexture = NULL;
	ID3D11Texture2D *pDepthStencilBufferTexture = NULL;

	// Fill swapchain description
	scd.BufferCount = 1;
	scd.BufferDesc.Width = windowMain.width;
	scd.BufferDesc.Height = windowMain.height;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	if (display.v_sync)
	{
		scd.BufferDesc.RefreshRate.Numerator = display.gpu_num;
		scd.BufferDesc.RefreshRate.Denominator = display.gpu_denom;
	}
	else
	{
		scd.BufferDesc.RefreshRate.Numerator = 0;
		scd.BufferDesc.RefreshRate.Denominator = 1;
	}
	if (display.triple_buff)
		scd.BufferCount = 3;

	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.OutputWindow = windowMain.hWnd;
	scd.SampleDesc.Count = 1;
	scd.SampleDesc.Quality = 0;
	scd.Windowed = !windowMain.fullscreen;
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
	txd.Width = windowMain.width;
	txd.Height = windowMain.height;
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
	viewport.Width = windowMain.width;
	viewport.Height = windowMain.height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	devcon->RSSetViewports(1, &viewport);

	// Create an alpha enabled blend state description
	bsd.RenderTarget[0].BlendEnable = TRUE;
	bsd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	bsd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	bsd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	bsd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	bsd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;	
	bsd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	bsd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	if (!Handle(&hr, HRH_ALPHABLEND_STATE, dev->CreateBlendState(&bsd, &blendstate)))
		return hr;
	devcon->OMSetBlendState(blendstate, 0, 0xffffffff);

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

	if (!CompileShader(&hr, L"main", &sh_main))
		return hr;
	if (!CompileShader(&hr, L"test", &sh_debug))
		return hr;
	if (!CompileShader(&hr, L"plain", &sh_plain))
		return hr;

	WriteToConsole(L"done\n");
	return S_OK;
}
HRESULT InitFonts()
{
	WriteToConsole(L"Loading fonts... ");

	if (!Handle(&hr, HRH_FONTS_CREATEDW1FACTORY, FW1CreateFactory(FW1_VERSION, &fw1factory)))
		return hr;

	if (!Handle(&hr, HRH_FONTS_CREATEWRAPPER, fw1factory->CreateFontWrapper(dev, L"Arial", &fw_arial)))
		return hr;
	if (!Handle(&hr, HRH_FONTS_CREATEWRAPPER, fw1factory->CreateFontWrapper(dev, L"Courier", &fw_courier)))
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
	mat_view = MLookAtLH(float3(0, 0, -1), float3(0, 0, 0), float3(0, 1, 0));
	mat_orthoview = MLookAtLH(float3(0, 0, -1), float3(0, 0, 0), float3(0, 1, 0));
	mat_proj = MPerspFovLH(DX_PI / 4, windowMain.aspect, 0.001f, 10000.0f);
	mat_orthoproj = MOrthoLH(windowMain.width, windowMain.height, 0.001f, 10000.0f);

	camera.moveToPoint(v3_origin + float3(0, 0, -1), -1);
	camera.lookAtPoint(v3_origin, -1);

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

	smartRelease(fw1factory);

	smartRelease(fw_arial);
	smartRelease(fw_courier);

	WriteToConsole(L"done\n");
}

void Log()
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
		WriteToConsole(to_wstring(timer.GetFPSStamp()) + L" FPS (" + to_wstring(timer.GetFramesCount()) + L") " + to_wstring(cpu_usage.lastUsage));

		//printf("\33[2K\r");
		//system("CLS");
		c = 0.0f;
	}
}
#include "Warnings.h"

#include <string>

#include "Main.h"
#include "Window.h"
#include "Frame.h"
#include "DebugWin.h"

using namespace std;

float delta = 0;

int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	ReadConfig();
#ifdef _DEBUG
	InitializeDebugConsole();
	WriteToConsole(L"---> START OF DEBUG CONSOLE <---\n");
#endif	

	CreateMainWindow(hInstance);
	ShowWindow(hWnd, nCmdShow);

	hr = InitD3D(hWnd);
	if (FAILED(hr))
	{
		wchar_t buffer[32] = {};
		swprintf(buffer,
			L"Unable to start D3DX11. Your hardware might not be compatible with the application requirements.\nError code: (HRESULT) 0x%X", hr);
		MessageBoxW(hWnd, buffer, L"Error", MB_OK);
		PostQuitMessage(0);
	}
	hr = RegisterRID();
	if (FAILED(hr))
	{
		wchar_t buffer[32] = {};
		swprintf(buffer,
			L"Unable to register input methods.\nError code: (HRESULT) 0x%X", hr);
		MessageBoxW(hWnd, buffer, L"Error", MB_OK);
		PostQuitMessage(0);
	}
	ShowCursor(false);
	ClipCursor(&wScreen);

	//

	MSG msg;
	bool run = 1;

	WriteToConsole(L"Entering main loop...\n");

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

		/*static bool once = 1;
		if (once)
		{
			once = 0;
		}
		else
			*/

		delta = timer.GetDelta();
		RenderFrameDX11(delta);

	#ifdef _DEBUG
		Log();
	#endif
		mouse.Reset();
		keys.Reset();
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

	wVSync = GetPrivateProfileIntW(L"settings", L"VSync", 0, L".\\config.ini");
	wFullscreen = GetPrivateProfileIntW(L"settings", L"Fullscreen", 0, L".\\config.ini");	

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
		wWidth = GetPrivateProfileIntW(L"settings", L"WindowWidth", 800, L".\\config.ini");
		wHeight = GetPrivateProfileIntW(L"settings", L"WindowHeight", 600, L".\\config.ini");
		wX = GetPrivateProfileIntW(L"settings", L"WindowX", 0, L".\\config.ini");
		wY = GetPrivateProfileIntW(L"settings", L"WindowY", 0, L".\\config.ini");
		wBorderless = GetPrivateProfileIntW(L"settings", L"Borderless", 0, L".\\config.ini");
		wAspectRatio = (float)wWidth / (float)wHeight;
	}

	wScreen = { wX, wY, wWidth + wX, wHeight + wY};

	return;

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
	hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(hr))
		return hr;

	// Use the factory to create an adapter for the primary graphics interface (video card).
	hr = factory->EnumAdapters(0, &adapter);
	if (FAILED(hr))
		return hr;

	// Enumerate the primary adapter output (monitor).
	hr = adapter->EnumOutputs(0, &adapterOutput);
	if (FAILED(hr))
		return hr;

	// Get the number of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format for the adapter output (monitor).
	hr = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if (FAILED(hr))
		return hr;

	// Create a list to hold all the possible display modes for this monitor/video card combination.
	displayModeList = new DXGI_MODE_DESC[numModes];
	if (FAILED(hr))
		return hr;

	// Now fill the display mode list structures.
	hr = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	if (FAILED(hr))
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
	hr = adapter->GetDesc(&adapterDesc);
	if (FAILED(hr))
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
	EnumHardware();

	WriteToConsole(L"Initializing D3D11... ");

	DXGI_SWAP_CHAIN_DESC scd;
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

	scd.BufferCount = 1;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.OutputWindow = hWnd;
	scd.SampleDesc.Count = 4;
	scd.Windowed = !wFullscreen;
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

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

	hr = D3D11CreateDeviceAndSwapChain(NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
	#ifdef _DEBUG
		D3D11_CREATE_DEVICE_DEBUG,
	#else
		0,
	#endif
		NULL,
		NULL,
		D3D11_SDK_VERSION,
		&scd,
		&swapchain,
		&dev,
		NULL,
		&devcon);
	if (FAILED(hr))
	{
		LogError(hr);
		return hr;
	}

	// Render target setup

	ID3D11Texture2D *pBackBufferTexture;
	swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBufferTexture);
	if (FAILED(hr))
	{
		LogError(hr);
		return hr;
	}

	dev->CreateRenderTargetView(pBackBufferTexture, NULL, &backbuffer); // use the back buffer address to create the render target
	if (FAILED(hr))
	{
		LogError(hr);
		return hr;
	}
	pBackBufferTexture->Release();

	devcon->OMSetRenderTargets(1, &backbuffer, NULL); // set the render target as the back buffer

	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	//viewport.TopLeftX = wX;
	//viewport.TopLeftY = wY;
	viewport.Width = wWidth;
	viewport.Height = wHeight;

	devcon->RSSetViewports(1, &viewport);

	WriteToConsole(L"done\n");

	LoadStartingFiles();

	return S_OK;
}
void CleanD3D()
{
	WriteToConsole(L"Cleaning D3D11... ");

	swapchain->SetFullscreenState(FALSE, NULL);
	swapchain->Release();
	dev->Release();
	devcon->Release();
	backbuffer->Release();

	WriteToConsole(L"done\n");

	ReleaseFiles();
}

HRESULT LoadStartingFiles()
{
	InitShaders();
	if (FAILED(hr))
		return hr;
	InitGraphics();
	if (FAILED(hr))
		return hr;

	keys.forward.vkey = 0x57;
	keys.backward.vkey = 0x53;
	keys.left.vkey = 0x41;
	keys.right.vkey = 0x44;

	player.moveToPoint(vec3(0, 0, -3), -1);

	return S_OK;
}
HRESULT InitShaders()
{
	WriteToConsole(L"Loading shaders... ");

	ID3D10Blob *blob = nullptr;

	hr = D3DX11CompileFromFileW(L".\\Shaders\\shader.hlsl", 0, 0, "VShader", "vs_4_0", 0, 0, 0, &blob, 0, 0);
	if (FAILED(hr))
	{
		LogError(hr);
		return hr;
	}
	hr = dev->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), NULL, &pVShader); // create the VS
	if (FAILED(hr))
	{
		LogError(hr);
		return hr;
	}
	hr = dev->CreateInputLayout(ied, 2, blob->GetBufferPointer(), blob->GetBufferSize(), &pLayout); // create an input layout from the VS
	if (FAILED(hr))
	{
		LogError(hr);
		return hr;
	}

	hr = D3DX11CompileFromFileW(L".\\Shaders\\shader.hlsl", 0, 0, "PShader", "ps_4_0", 0, 0, 0, &blob, 0, 0);
	if (FAILED(hr))
	{
		LogError(hr);
		return hr;
	}	
	hr = dev->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), NULL, &pPShader); // create the PS
	if (FAILED(hr))
	{
		LogError(hr);
		return hr;
	}

	//hr = LoadShader(L".\\Shaders\\toon3_cel.hlsl", pCelVS, pCelPS, &vs_blob, &ps_blob);
	//dev->CreateInputLayout(ied_VS_INPUT, 5, vs_blob->GetBufferPointer(), vs_blob->GetBufferSize(), &pCelLayout);
	//hr = LoadShader(L".\\Shaders\\toon3_outline.hlsl", pOutlineVS, pOutlinePS, &vs_blob, &ps_blob);
	//dev->CreateInputLayout(ied_VS_INPUT, 4, vs_blob->GetBufferPointer(), vs_blob->GetBufferSize(), &pCelLayout);

	WriteToConsole(L"done\n");
	return S_OK;
}
HRESULT InitGraphics()
{
	WriteToConsole(L"Loading starting graphics... ");

	D3D11_BUFFER_DESC bd; // buffer description
	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX) * 64;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	hr = dev->CreateBuffer(&bd, NULL, &pVertexBuffer);
	if (FAILED(hr))
	{
		LogError(hr);
		return hr;
	}

	bd.ByteWidth = sizeof(D3DXMATRIX) * 3;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	hr = dev->CreateBuffer(&bd, NULL, &pConstantBuffer);
	if (FAILED(hr))
	{
		LogError(hr);
		return hr;
	}

	bd.ByteWidth = sizeof(UINT) * 64;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	hr = dev->CreateBuffer(&bd, NULL, &pIndexBuffer);
	if (FAILED(hr))
	{
		LogError(hr);
		return hr;
	}

	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	devcon->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);
	devcon->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	devcon->VSSetConstantBuffers(0, 1, &pConstantBuffer);

	//

	D3DXMatrixIdentity(&mIdentity);
	mWorld = mIdentity;
	mView = mIdentity;
	D3DXMatrixLookAtLH(&mView, &(vec3(0, 0, -1)), &(vec3(0, 0, 0)), &vec3(0, 1, 0));
	D3DXMatrixPerspectiveFovLH(&mProj, D3DX_PI / 4, wAspectRatio, 0.0, 1.0);

	WriteToConsole(L"done\n");
	return S_OK;
}
void ReleaseFiles()
{
	WriteToConsole(L"Releasing files... ");

	smartRelease(pVShader);
	smartRelease(pPShader);
	smartRelease(pCelVS);
	smartRelease(pCelPS);
	smartRelease(pOutlineVS);
	smartRelease(pOutlinePS);

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
			WriteToConsole(to_wstring(mouse.GetButtonState(leftbutton)));
			WriteToConsole(L" (LMB) ");
			WriteToConsole(to_wstring(mouse.GetButtonState(middlebutton)));
			WriteToConsole(L" (MMB) ");
			WriteToConsole(to_wstring(mouse.GetButtonState(rightbutton)));
			WriteToConsole(L" (RMB) ");

			WriteToConsole(to_wstring(keys.forward.press));
			WriteToConsole(L" (W) ");
			WriteToConsole(to_wstring(keys.left.press));
			WriteToConsole(L" (A) ");
			WriteToConsole(to_wstring(keys.backward.press));
			WriteToConsole(L" (S) ");
			WriteToConsole(to_wstring(keys.right.press));
			WriteToConsole(L" (D) ");
			
			//printf("\33[2K\r");
			//system("CLS");
			c = 0.0f;
		}		
	}
}
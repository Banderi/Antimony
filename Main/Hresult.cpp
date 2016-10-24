#include <string>
#include <comdef.h>

#include "Hresult.h"
#include "Window.h"
#include "DebugWin.h"
#include "Warnings.h"

bool Handle(HRESULT *hOut, DWORD facing, HRESULT hr)
{
	*hOut = hr;

	if (FAILED(hr))
	{
		LogError(hr);

		wchar_t buffer[512] = {};
		_com_error err(hr);
		LPCTSTR errMsg = err.ErrorMessage();

		switch (facing)
		{
		case HRH_MAIN_ENUMHW:
			swprintf(buffer,
				L"Hardware enumeration failed! Program will now terminate.");
			break;
		case HRH_MAIN_INITD3D:
			swprintf(buffer,
				L"DirectX11 initialization failed! Program will now terminate.");
			break;
		case HRH_MAIN_REGHID:
			swprintf(buffer,
				L"HID objects registration failed! Program will now terminate.");
			break;
		case HRH_MAIN_INITSHADERS:
			swprintf(buffer,
				L"Shaders initialization failed! Program will now terminate.");
			break;
		case HRH_MAIN_INITGRAPHICS:
			swprintf(buffer,
				L"Graphics initialization failed! Program will now terminate.");
			break;
		case HRH_MAIN_STARTINGFILES:
			swprintf(buffer,
				L"Could not load starting files! Program will now terminate.");
			break;

		// EnumHardware(...)
		case HRH_ENUM_CREATEDXGIFACTORY:
			swprintf(buffer,
				L"Unable to create DirectX graphics interface factory.\nError code: %s (0x%X)", errMsg, hr);
		case HRH_ENUM_ENUMGPU:
			swprintf(buffer,
				L"Unable to enumerate graphic adapter.\nError code: %s (0x%X)", errMsg, hr);
		case HRH_ENUM_ENUMOUTPUTDEVICE:
			swprintf(buffer,
				L"Unable to enumerate output device.\nError code: %s (0x%X)", errMsg, hr);
		case HRH_ENUM_GETOUTPUTMODESNUMBER:
			swprintf(buffer,
				L"Unable to enumerate output modes count.\nError code: %s (0x%X)", errMsg, hr);
		case HRH_ENUM_FILLOUTPUTMODESLIST:
			swprintf(buffer,
				L"Unable to fill output modes list.\nError code: %s (0x%X)", errMsg, hr);
		case HRH_ENUM_GETGPUDESC:
			swprintf(buffer,
				L"Unable to get GPU description.\nError code: %s (0x%X)", errMsg, hr);

		// InitD3D(...)
		case HRH_SWAPCHAIN_CREATE:
			swprintf(buffer,
				L"Unable to create swapchain.\nError code: %s (0x%X)", errMsg, hr);
			break;
		case HRH_SWAPCHAIN_SURFACEBUFFER:
			swprintf(buffer,
				L"Unable to create surface buffer.\nError code: %s (0x%X)", errMsg, hr);
			break;
		case HRH_RENDERTARGET_CREATE:
			swprintf(buffer,
				L"Unable to create render target.\nError code: %s (0x%X)", errMsg, hr);
			break;
		case HRH_DEPTHSTENCIL_TEXTURE:
			swprintf(buffer,
				L"Unable to create depth stencil texture.\nError code: %s (0x%X)", errMsg, hr);
			break;
		case HRH_DEPTHSTENCIL_STATE:
			swprintf(buffer,
				L"Unable to create depth stencil state.\nError code: %s (0x%X)", errMsg, hr);
			break;
		case HRH_DEPTHSTENCIL_VIEW:
			swprintf(buffer,
				L"Unable to create depth stencil view.\nError code: %s (0x%X)", errMsg, hr);
			break;
		case HRH_RASTERIZER_STATE:
			swprintf(buffer,
				L"Unable to create rasterizer state.\nError code: %s (0x%X)", errMsg, hr);
			break;

		// InitShaders(...)
		case HRH_SHADER_COMPILE:
			swprintf(buffer,
				L"Unable to compile shader from file.\nError code: %s (0x%X)", errMsg, hr);
			break;
		case HRH_SHADER_CREATE:
			swprintf(buffer,
				L"Unable to create shader.\nError code: %s (0x%X)", errMsg, hr);
			break;
		case HRH_SHADER_INPUTLAYOUT:
			swprintf(buffer,
				L"Unable to create input layout from shader.\nError code: %s (0x%X)", errMsg, hr);
			break;

		// InitGraphics(...)
		case HRH_GRAPHICS_VERTEXBUFFER:
			swprintf(buffer,
				L"Unable to create vertex buffer.\nError code: %s (0x%X)", errMsg, hr);
			break;
		case HRH_GRAPHICS_CONSTANTBUFFER:
			swprintf(buffer,
				L"Unable to create constant buffer.\nError code: %s (0x%X)", errMsg, hr);
			break;
		case HRH_GRAPHICS_INDEXBUFFER:
			swprintf(buffer,
				L"Unable to create index buffer.\nError code: %s (0x%X)", errMsg, hr);
			break;
		}

		MessageBoxW(hWnd, buffer, L"Error", MB_OK);

		return false;
	}
	return true;
}

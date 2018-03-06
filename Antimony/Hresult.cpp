#include <comdef.h>		// required for _com_error				(Antimony::handleErr)

#include "Debug.h"
#include "Hresult.h"
#include "Param.h"
#include "Console.h"

///

HRESULT hr;

///

bool Antimony::handleErr(HRESULT *hOut, DWORD facing, HRESULT hr, const wchar_t* opt)
{
	if (hOut != nullptr)
		*hOut = hr;

	if (FAILED(hr))
	{
		Antimony::logError(hr);

		wchar_t buffer[512] = {};
		_com_error err(hr);
		LPCTSTR errMsg = err.ErrorMessage();

		switch (facing)
		{
		case HRH_MAIN_DEBUGMONIOR:
			swprintf_s(buffer,
				L"Debug monitor creation failed! Program will now terminate.");
			break;
		case HRH_MAIN_ENUMHW:
			swprintf_s(buffer,
				L"Hardware enumeration failed! Program will now terminate.");
			break;
		case HRH_MAIN_INITD3D:
			swprintf_s(buffer,
				L"DirectX11 initialization failed! Program will now terminate.");
			break;
		case HRH_MAIN_REGHID:
			swprintf_s(buffer,
				L"HID objects registration failed! Program will now terminate.");
			break;
		case HRH_MAIN_INITSHADERS:
			swprintf_s(buffer,
				L"Shaders initialization failed! Program will now terminate.");
			break;
		case HRH_MAIN_INITFONTS:
			swprintf_s(buffer,
				L"Fonts initialization failed! Program will now terminate.");
			break;
		case HRH_MAIN_INITGRAPHICS:
			swprintf_s(buffer,
				L"Graphics initialization failed! Program will now terminate.");
			break;
		case HRH_MAIN_INITPHYSICS:
			swprintf_s(buffer,
				L"Physics initialization failed! Program will now terminate.");
			break;
		case HRH_MAIN_STARTINGFILES:
			swprintf_s(buffer,
				L"Could not load starting files! Program will now terminate.");
			break;
		case HRH_MAIN_ASSETLOADERS:
			swprintf_s(buffer,
				L"Could not load set up asset loaders! Program will now terminate.");
			break;

			// EnumHardware(...)
		case HRH_ENUM_CREATEDXGIFACTORY:
			swprintf_s(buffer,
				L"Unable to create DirectX graphics interface factory.\nError code: %s (0x%X)", errMsg, hr);
			break;
		case HRH_ENUM_ENUMGPU:
			swprintf_s(buffer,
				L"Unable to enumerate graphic adapter.\nError code: %s (0x%X)", errMsg, hr);
			break;
		case HRH_ENUM_ENUMOUTPUTDEVICE:
			swprintf_s(buffer,
				L"Unable to enumerate output device.\nError code: %s (0x%X)", errMsg, hr);
			break;
		case HRH_ENUM_GETOUTPUTMODESNUMBER:
			swprintf_s(buffer,
				L"Unable to enumerate output modes count.\nError code: %s (0x%X)", errMsg, hr);
			break;
		case HRH_ENUM_FILLOUTPUTMODESLIST:
			swprintf_s(buffer,
				L"Unable to fill output modes list.\nError code: %s (0x%X)", errMsg, hr);
			break;
		case HRH_ENUM_GETGPUDESC:
			swprintf_s(buffer,
				L"Unable to get GPU description.\nError code: %s (0x%X)", errMsg, hr);
			break;
		case HRH_ENUM_GETMEMINFO:
			swprintf_s(buffer,
				L"Unable to get memory info.\nError code: %s (0x%X)", errMsg, hr);
			break;

			// InitD3D(...)
		case HRH_SWAPCHAIN_CREATE:
			swprintf_s(buffer,
				L"Unable to create swapchain.\nError code: %s (0x%X)", errMsg, hr);
			break;
		case HRH_SWAPCHAIN_SURFACEBUFFER:
			swprintf_s(buffer,
				L"Unable to create surface buffer.\nError code: %s (0x%X)", errMsg, hr);
			break;
		case HRH_RENDERTARGET_CREATE:
			swprintf_s(buffer,
				L"Unable to create render target.\nError code: %s (0x%X)", errMsg, hr);
			break;
		case HRH_DEPTHSTENCIL_TEXTURE:
			swprintf_s(buffer,
				L"Unable to create depth stencil texture.\nError code: %s (0x%X)", errMsg, hr);
			break;
		case HRH_DEPTHSTENCIL_STATE:
			swprintf_s(buffer,
				L"Unable to create depth stencil state.\nError code: %s (0x%X)", errMsg, hr);
			break;
		case HRH_DEPTHSTENCIL_VIEW:
			swprintf_s(buffer,
				L"Unable to create depth stencil view.\nError code: %s (0x%X)", errMsg, hr);
			break;
		case HRH_RASTERIZER_STATE:
			swprintf_s(buffer,
				L"Unable to create rasterizer state.\nError code: %s (0x%X)", errMsg, hr);
			break;
		case HRH_ALPHABLEND_STATE:
			swprintf_s(buffer,
				L"Unable to create alpha blending state.\nError code: %s (0x%X)", errMsg, hr);
			break;

			// InitShaders(...)
		case HRH_SHADER_COMPILE:
			swprintf_s(buffer,
				L"Unable to compile shader from file.\nError code: %s (0x%X)", errMsg, hr);
			break;
		case HRH_SHADER_CREATE:
			swprintf_s(buffer,
				L"Unable to create shader.\nError code: %s (0x%X)", errMsg, hr);
			break;
		case HRH_SHADER_INPUTLAYOUT:
			swprintf_s(buffer,
				L"Unable to create input layout from shader.\nError code: %s (0x%X)", errMsg, hr);
			break;
		case HRH_SHADER_VERTEXBUFFER:
			swprintf_s(buffer,
				L"Unable to create shader's vertex buffer.\nError code: %s (0x%X)", errMsg, hr);
			break;

			// InitFonts(...)
		case HRH_FONTS_CREATEDW1FACTORY:
			swprintf_s(buffer,
				L"Unable to create font wrapper factory.\nError code: %s (0x%X)", errMsg, hr);
			break;
		case HRH_FONTS_CREATEDW1GEOMETRY:
			swprintf_s(buffer,
				L"Unable to create font wrapper factory.\nError code: %s (0x%X)", errMsg, hr);
			break;
		case HRH_FONTS_CREATEWRAPPER:
			swprintf_s(buffer,
				L"Unable to create font wrapper for font \"%s\".\nError code: %s (0x%X)", opt, errMsg, hr);
			break;

			// InitGraphics(...)
		case HRH_GRAPHICS_VERTEXBUFFER:
			swprintf_s(buffer,
				L"Unable to create vertex buffer.\nError code: %s (0x%X)", errMsg, hr);
			break;
		case HRH_GRAPHICS_CONSTANTBUFFER:
			swprintf_s(buffer,
				L"Unable to create constant buffer.\nError code: %s (0x%X)", errMsg, hr);
			break;
		case HRH_GRAPHICS_INDEXBUFFER:
			swprintf_s(buffer,
				L"Unable to create index buffer.\nError code: %s (0x%X)", errMsg, hr);
			break;

			// Antimony::FillBuffer(...)
		case HRH_BUFFER_MAPPING:
			swprintf_s(buffer,
				L"Unable to map buffer data.\nError code: %s (0x%X)", errMsg, hr);
			break;
		case HRH_BUFFER_OVERFLOW:
			swprintf_s(buffer,
				L"Unable to fill buffer with data.\nError code: %s (0x%X)", errMsg, hr);
			break;
		}

		MessageBoxW(Antimony::window_main.hWnd, buffer, L"Error", MB_OK);

		return false;
	}
	return true;
}
void Antimony::logError(HRESULT hr)
{
	wchar_t buf[64];

	/*log(L"ERROR: 0x", CSL_ERR_FATAL, false);
	std::wostringstream oss;
	oss << std::hex << hr;
	log(oss.str() + L"\n", CSL_ERR_FATAL, false);*/

	swprintf_s(buf, L"ERROR: 0x%X\n", hr);
	log(buf, CSL_ERR_FATAL, false);
}
#include <io.h>			// required for _open_osfhandle			(Antimony::initializeDebugConsole)
#include <fcntl.h>		// required for _O_TEXT					(Antimony::initializeDebugConsole)
#include <comdef.h>		// required for _com_error				(Antimony::handleErr)
#include <sstream>		// required for std::wostringstream		(Antimony::logError)

#include "Warnings.h"
#include "Antimony.h"
#include "Hresult.h"

///

void Antimony::initializeDebugConsole()
{
	//Create a console for this application
	AllocConsole();

	//Redirect unbuffered STDOUT to the console
	HANDLE ConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	int SystemOutput = _open_osfhandle(intptr_t(ConsoleOutput), _O_TEXT);
	FILE *COutputHandle = _fdopen(SystemOutput, "w");
	*stdout = *COutputHandle;
	setvbuf(stdout, NULL, _IONBF, 0);

	//Redirect unbuffered STDERR to the console
	HANDLE ConsoleError = GetStdHandle(STD_ERROR_HANDLE);
	int SystemError = _open_osfhandle(intptr_t(ConsoleError), _O_TEXT);
	FILE *CErrorHandle = _fdopen(SystemError, "w");
	*stderr = *CErrorHandle;
	setvbuf(stderr, NULL, _IONBF, 0);

	//Redirect unbuffered STDIN to the console
	HANDLE ConsoleInput = GetStdHandle(STD_INPUT_HANDLE);
	int SystemInput = _open_osfhandle(intptr_t(ConsoleInput), _O_TEXT);
	FILE *CInputHandle = _fdopen(SystemInput, "r");
	*stdin = *CInputHandle;
	setvbuf(stdin, NULL, _IONBF, 0);

	//make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog point to console as well
	std::ios::sync_with_stdio(true);

	HWND consoleWindow = GetConsoleWindow();

	SetWindowPos(consoleWindow, 0, 1950, 500, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}
void Antimony::shutdownDebugConsole()
{
	//Write "Press any key to exit"
	HANDLE ConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD CharsWritten;
	WriteConsoleW(ConsoleOutput, L"\nPress any key to exit", 22, &CharsWritten, 0);

	//Disable line-based input mode so we can get a single character
	HANDLE ConsoleInput = GetStdHandle(STD_INPUT_HANDLE);
	SetConsoleMode(ConsoleInput, 0);

	//Read a single character
	TCHAR InputBuffer;
	DWORD CharsRead;
	ReadConsoleW(ConsoleInput, &InputBuffer, 1, &CharsRead, 0);
}
HRESULT Antimony::writeToConsole(std::wstring string, bool t, bool logf)
{
	//char buf[32];
	//std::time_t timestamp = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	//ctime_s(buf, 32, &timestamp);

	char buf[1000];
	time_t ts = time(NULL);
	struct tm p;
	localtime_s(&p, &ts);
	strftime(buf, 1000, "[%H:%M:%S] ", &p);
	printf("%s", buf);

	if (logf)
	{
		if (t)
			m_logfile << buf << std::flush;
		m_logfile << string.c_str() << std::flush;
	}

	if (game.debug == true)
	{
		HANDLE ConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
		DWORD CharsWritten;
		if (ConsoleOutput != NULL)
		{
			BOOL er = WriteConsoleW(ConsoleOutput, string.c_str(), string.size(), &CharsWritten, 0);

			if (er == 0)
				return HRESULT_FROM_WIN32(GetLastError());
			else
				return S_OK;
		}
		else
			return S_OK;
	}
	else
		return S_OK;
}
void Antimony::consoleLog()
{
	static float c = 0.0f;
	c += m_delta;

	if (c >= 0.05)
	{
		/*writeToConsole(L"\r                                    \r");
		writeToConsole(L"Mouse position: ");
		writeToConsole(to_wstring(mouse.GetCoord(xcoord).getPos()));
		writeToConsole(L" (X) ");
		writeToConsole(to_wstring(mouse.GetCoord(ycoord).getPos()));
		writeToConsole(L" (X)");*/

		//writeToConsole(L"\r                                                            \r");
		////writeToConsole(to_wstring(timer.GetFPSStamp()) + L" FPS (" + to_wstring(timer.GetFramesCount()) + L") " + to_wstring(cpu_usage.lastUsage));
		//writeToConsole(to_wstring(mouse.X.getPos()));
		//writeToConsole(L" (X) ");
		//writeToConsole(to_wstring(mouse.Y.getPos()));
		//writeToConsole(L" (Y)");

		writeToConsole(L"\r                                     \r", false, false);
		writeToConsole(L"Camera: ", false, false);
		writeToConsole(std::to_wstring(game.dbg_entityfollow), false, false);
		writeToConsole(L" (entity ", false, false);
		writeToConsole(std::to_wstring(game.dbg_entityfollow%m_physEntities.size()), false, false);
		writeToConsole(L")", false, false);

		//printf("\33[2K\r");
		//system("CLS");
		c = 0.0f;
	}
}
void Antimony::logError(HRESULT hr)
{
	writeToConsole(L"ERROR: 0x", false);
	std::wostringstream oss;
	oss << std::hex << hr;
	writeToConsole(oss.str(), false);
	//writeToConsole(std::to_wstring(hr));
	writeToConsole(L"\n", false);
}
bool Antimony::handleErr(HRESULT *hOut, DWORD facing, HRESULT hr, const wchar_t* opt)
{
	*hOut = hr;

	if (FAILED(hr))
	{
		logError(hr);

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
		case HRH_MAIN_INITFONTS:
			swprintf(buffer,
				L"Fonts initialization failed! Program will now terminate.");
			break;
		case HRH_MAIN_INITGRAPHICS:
			swprintf(buffer,
				L"Graphics initialization failed! Program will now terminate.");
			break;
		case HRH_MAIN_INITPHYSICS:
			swprintf(buffer,
				L"Physics initialization failed! Program will now terminate.");
			break;
		case HRH_MAIN_STARTINGFILES:
			swprintf(buffer,
				L"Could not load starting files! Program will now terminate.");
			break;

			// EnumHardware(...)
		case HRH_ENUM_CREATEDXGIFACTORY:
			swprintf(buffer,
				L"Unable to create DirectX graphics interface factory.\nError code: %s (0x%X)", errMsg, hr);
			break;
		case HRH_ENUM_ENUMGPU:
			swprintf(buffer,
				L"Unable to enumerate graphic adapter.\nError code: %s (0x%X)", errMsg, hr);
			break;
		case HRH_ENUM_ENUMOUTPUTDEVICE:
			swprintf(buffer,
				L"Unable to enumerate output device.\nError code: %s (0x%X)", errMsg, hr);
			break;
		case HRH_ENUM_GETOUTPUTMODESNUMBER:
			swprintf(buffer,
				L"Unable to enumerate output modes count.\nError code: %s (0x%X)", errMsg, hr);
			break;
		case HRH_ENUM_FILLOUTPUTMODESLIST:
			swprintf(buffer,
				L"Unable to fill output modes list.\nError code: %s (0x%X)", errMsg, hr);
			break;
		case HRH_ENUM_GETGPUDESC:
			swprintf(buffer,
				L"Unable to get GPU description.\nError code: %s (0x%X)", errMsg, hr);
			break;
		case HRH_ENUM_GETMEMINFO:
			swprintf(buffer,
				L"Unable to get memory info.\nError code: %s (0x%X)", errMsg, hr);
			break;

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
		case HRH_ALPHABLEND_STATE:
			swprintf(buffer,
				L"Unable to create alpha blending state.\nError code: %s (0x%X)", errMsg, hr);
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

			// InitFonts(...)
		case HRH_FONTS_CREATEDW1FACTORY:
			swprintf(buffer,
				L"Unable to create font wrapper factory.\nError code: %s (0x%X)", errMsg, hr);
			break;
		case HRH_FONTS_CREATEWRAPPER:
			swprintf(buffer,
				L"Unable to create font wrapper for font \"%s\".\nError code: %s (0x%X)", opt, errMsg, hr);
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

		MessageBoxW(window_main.hWnd, buffer, L"Error", MB_OK);

		return false;
	}
	return true;
}
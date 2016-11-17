#include <Windows.h>
#include <fcntl.h>
#include <io.h>
#include <fstream>
#include <string>

#include "DebugWin.h"
#include "Param.h"

//

void InitializeDebugConsole()
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
void ShutdownDebugConsole()
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

HRESULT WriteToConsole(std::wstring string)
{
	if (game.debug == true)
	{
		HANDLE ConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
		DWORD CharsWritten;
		BOOL er = WriteConsoleW(ConsoleOutput, string.c_str(), string.size(), &CharsWritten, 0);

		if (er == 0)
			return HRESULT_FROM_WIN32(GetLastError());
		else
			return S_OK;
	}
	else
		return S_OK;
}
void LogError(HRESULT hr)
{
	WriteToConsole(L"ERROR: ");
	WriteToConsole(std::to_wstring(hr));
	WriteToConsole(L"\n");
}


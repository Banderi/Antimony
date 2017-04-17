#include <io.h>			// required for _open_osfhandle			(Antimony::initializeDebugConsole)
#include <fcntl.h>		// required for _O_TEXT					(Antimony::initializeDebugConsole)
#include <sstream>		// required for std::wostringstream		(Antimony::logError)

#include "Warnings.h"
#include "Antimony.h"
#include "Console.h"

///

HRESULT Antimony::initDebugMonitor()
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

	return S_OK;
}
void Antimony::unacquireDebugMonitor()
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
void Antimony::monitorLog()
{
	static float c = 0.0f;
	c += m_delta;

	if (c >= 0.1)
	{
		/*log(L"\r                                    \r");
		log(L"Mouse position: ");
		log(to_wstring(mouse.GetCoord(xcoord).getPos()));
		log(L" (X) ");
		log(to_wstring(mouse.GetCoord(ycoord).getPos()));
		log(L" (X)");*/

		//log(L"\r                                                            \r");
		////log(to_wstring(timer.GetFPSStamp()) + L" FPS (" + to_wstring(timer.GetFramesCount()) + L") " + to_wstring(cpu_usage.lastUsage));
		//log(to_wstring(mouse.X.getPos()));
		//log(L" (X) ");
		//log(to_wstring(mouse.Y.getPos()));
		//log(L" (Y)");

		logVolatile(L"\r                                     \r");
		logVolatile(L"Camera: ");
		logVolatile(std::to_wstring(game.dbg_entityfollow));
		logVolatile(L" (entity ");
		logVolatile(std::to_wstring(game.dbg_entityfollow%m_physEntities.size()));
		logVolatile(L") -- ");

		//printf("\33[2K\r");
		//system("CLS");
		c = 0.0f;
	}
}
HRESULT Antimony::log(std::wstring string, unsigned int col, bool timestamp)
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

	if (timestamp)
		m_logFile << buf << std::flush;
	m_logFile << string.c_str() << std::flush;
	devConsole.log(string, col);

	return logVolatile(string);
}
HRESULT Antimony::logVolatile(std::wstring string)
{
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
void Antimony::logError(HRESULT hr)
{
	/*log(L"ERROR: 0x", CSL_ERR_FATAL, false);
	std::wostringstream oss;
	oss << std::hex << hr;
	log(oss.str() + L"\n", CSL_ERR_FATAL, false);*/

	swprintf(m_globalStr64, L"ERROR: 0x%X\n", hr);
	log(m_globalStr64, CSL_ERR_FATAL, false);
}
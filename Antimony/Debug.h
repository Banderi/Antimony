#pragma once

#include <Windows.h>	// required for HRESULT
#include <string>		// required for std::wstring			(Antimony::log)
#include <fstream>		// required for std::wofstream			(Antimony::logfile)

///

namespace Antimony
{
	extern std::wofstream logFile;

	HRESULT initDebugMonitor();
	void unacquireDebugMonitor();
	void monitorLog();
	HRESULT log(std::wstring string, unsigned int col, bool timestamp = true);
	HRESULT logVolatile(std::wstring string);
}
#pragma once

#include <string>
#include <Shlwapi.h>			// required for PathRemoveFileSpec()

///

#define ABSOLUTE_PATH false

///

namespace Antimony
{
	extern bool use_absolute_paths;
	extern std::wstring process_modulename;
	extern std::wstring process_fullexename;
	extern std::wstring process_fullpath;
	extern std::wstring process_workingdir;

	void buildPaths();
	std::wstring filePath(std::wstring relpath, std::wstring filename);
	std::wstring filePath(std::wstring filename);
}
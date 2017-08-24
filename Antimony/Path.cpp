#include <string>
#include <Shlwapi.h>			// required for PathRemoveFileSpec()

#include "Path.h"

///

void Antimony::BuildPaths()
{
	WCHAR *temp = new WCHAR[MAX_PATH];

	GetModuleFileName(GetModuleHandleW(NULL), temp, MAX_PATH);
	process_fullpath = temp;

	PathRemoveFileSpec(temp);
	process_workingdir = temp;

	temp = PathFindFileName(process_fullpath.c_str());
	process_fullexename = temp;

	*(PathFindExtension(temp)) = 0;
	process_modulename = temp;
}
std::wstring Antimony::FilePath(std::wstring relpath, std::wstring filename)
{
	std::wstring fullpath;
	if (!use_absolute_paths)
		fullpath = L"." + relpath + filename;
	else
		fullpath = Antimony::process_workingdir + relpath + filename;

	return fullpath;
}
std::wstring Antimony::FilePath(std::wstring filename)
{
	std::wstring fullpath;
	if (!use_absolute_paths)
		return filename;
	else
		fullpath = Antimony::process_workingdir + filename;
	return fullpath;
}

namespace Antimony
{
	bool use_absolute_paths = true;
	std::wstring process_modulename;
	std::wstring process_fullexename;
	std::wstring process_fullpath;
	std::wstring process_workingdir;
}
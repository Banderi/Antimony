#pragma once

#include <windows.h>	// required for various WinSDK typedefs
#include <vector>

///

class CpuUsage
{
private:
	//system total times
	FILETIME m_ftPrevSysKernel;
	FILETIME m_ftPrevSysUser;

	//process times
	FILETIME m_ftPrevProcKernel;
	FILETIME m_ftPrevProcUser;

	short m_nCpuUsage;
	ULONGLONG m_dwLastRun;
	volatile LONG m_lRunCount;

	unsigned char m_maxRecords;

	ULONGLONG SubtractTimes(const FILETIME& ftA, const FILETIME& ftB);
	bool EnoughTimePassed(bool costant = 0);
	inline bool IsFirstRun() const { return (m_dwLastRun == 0); }

public:
	short lastUsage;
	std::vector<unsigned short> usageStream;

	short GetUsage(bool costant = 0);
	void SetMaxRecords(char c);
	char GetMaxRecords();

	CpuUsage(void);
};
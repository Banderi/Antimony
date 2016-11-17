#pragma once

#include <windows.h>
#include <vector>
#include <Psapi.h>

class CpuUsage
{

public:
	CpuUsage(void);
	short GetUsage(bool costant = 0);
	void SetMaxRecords(char c);
	char GetMaxRecords();

	short lastUsage;
	std::vector<unsigned short> usageStream;

private:
	ULONGLONG SubtractTimes(const FILETIME& ftA, const FILETIME& ftB);
	bool EnoughTimePassed(bool costant = 0);
	inline bool IsFirstRun() const { return (m_dwLastRun == 0); }

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
};

extern CpuUsage cpu_usage;

extern DWORDLONG totalPhysMem, physMemAvail;
extern ULONG_PTR physMemUsedByMe;
extern MEMORYSTATUSEX memInfo;
extern PROCESS_MEMORY_COUNTERS_EX pmc;
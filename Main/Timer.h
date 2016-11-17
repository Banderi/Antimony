#pragma once

class Timer
{
	double m_previousClock[32];
	double m_currentClock[32];
	double m_delta[32];
	double m_correction;
	double m_clockStamp;
	int m_framesCount;
	float m_fpsStamp;

public:
	void UpdateDelta(char d);
	void SetCorrection(float c);
	double GetDelta(char d);
	int GetFramesCount();
	float GetFPSStamp();

	float minFps, maxFps;

	Timer();
};
extern Timer timer;

#define TIMER_FRAME_GLOBAL	0x00000000
#define TIMER_TEST			0x00000001
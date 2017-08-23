#pragma once

#include <vector>

///

#define TIMER_FRAME_GLOBAL		0x00000000
#define TIMER_PRESTEP			0x00000001
#define TIMER_RENDER_ENTITIES	0x00000010
#define TIMER_RENDER_WORLD		0x00000011
#define TIMER_RENDER_HUD		0x00000012
#define TIMER_RENDER_DEBUG		0x00000013
#define TIMER_AFTERSTEP			0x00000020

#define TIMER_MAXSTAMPS		512

///

class Timer
{
	double m_previousClock[TIMER_MAXSTAMPS];
	double m_currentClock[TIMER_MAXSTAMPS];
	double delta[TIMER_MAXSTAMPS];
	unsigned short m_stampsCount;
	double m_clocksPerSecond;
	//double m_clockLastSecond;
	int m_framesCount;
	float m_currentFPS;

public:
	std::vector<double> fps_history;

	void catchTime(unsigned short d);
	void setCorrection(float c);
	double getDelta(unsigned short d);
	double getLocalDelta(unsigned short d);
	int getFramesCount();
	float getFPSStamp();

	float minFps, maxFps;

	Timer();
};

namespace Antimony
{
	extern Timer timer;
}
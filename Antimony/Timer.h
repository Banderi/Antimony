#pragma once

#include <vector>

///

#define TIMER_FRAME_GLOBAL		0
#define TIMER_PRESTEP			1
#define TIMER_RENDER_ENTITIES	2
#define TIMER_RENDER_WORLD		3
#define TIMER_RENDER_HUD		4
#define TIMER_RENDER_DEBUG		5
#define TIMER_AFTERSTEP			6

#define TIMER_MAXSTAMPS			128
#define TIMER_END				TIMER_MAXSTAMPS - 1

///

class Timer
{
	std::vector<clock_t> m_previousClock;
	std::vector<clock_t> m_currentClock;
	std::vector<double> m_derivClock;
	clock_t m_clocksPerSecond;
	int m_framesCount;
	float m_currentFPS;

public:
	double lastDelta;
	std::vector<double> fps_history;

	void catchTime(int d);
	void setCorrection(float c);
	double getStep(int d1, int d2, bool smooth = true);
	double getDelta(int d1 = TIMER_FRAME_GLOBAL, int d2 = TIMER_FRAME_GLOBAL);
	int getFramesCount();
	float getFPSStamp();

	float minFps, maxFps;

	Timer();
};

namespace Antimony
{
	extern Timer timer;
}
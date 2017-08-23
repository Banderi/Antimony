#include <ctime>				// required for clock() and CLOCKS_PER_SEC					(Timer::UpdateDelta; Timer::GetFPSStamp)
#include <Shlwapi.h>			// required for abs()										(Timer::UpdateDelta)

#include "Timer.h"

///

void Timer::catchTime(unsigned short d)
{
	if (d > TIMER_MAXSTAMPS - 1)
		d = TIMER_MAXSTAMPS - 1;
	if (m_stampsCount < d)
		m_stampsCount = d;

	m_previousClock[d] = m_currentClock[d];
	m_currentClock[d] = clock();
	delta[d] = abs((long)(m_currentClock[d] - m_previousClock[d])) / m_clocksPerSecond;

	static double clockLastSecond = m_currentClock[d] - m_clocksPerSecond;

	if (d == TIMER_FRAME_GLOBAL)
	{
		m_framesCount++;

		if (m_currentClock[d] >= clockLastSecond + m_clocksPerSecond)
		{
			/*float fdiff = (m_currentClock[d] - (m_clockLastSecond + m_clocksPerSecond))
				/ (m_currentClock[d] - m_previousClock[d]);
			m_fpsStamp = m_framesCount - fdiff;*/

			m_framesCount = 0;
			clockLastSecond += m_clocksPerSecond;
		}

		static double clockLastStamp = m_currentClock[d] - 200;
		static int frames200Stamp = 0;
		frames200Stamp++;

		if (m_currentClock[d] >= clockLastStamp + 200)
		{
			auto frameAverageTime = frames200Stamp / (m_currentClock[d] - clockLastStamp);
			m_currentFPS = m_clocksPerSecond * frameAverageTime;

			if (maxFps < m_currentFPS)
				maxFps = m_currentFPS;
			if (minFps > m_currentFPS)
				minFps = m_currentFPS;
			if (minFps == -1)
				minFps = 60;

			clockLastStamp = m_currentClock[d];
			frames200Stamp = 0;
		}

		double last_delta = delta[d] * 1000;
		if (last_delta<0)
			last_delta = 0;
		if (last_delta>100)
			last_delta = 100;
		fps_history.push_back(last_delta);
		if (fps_history.size() > 100)
			fps_history.erase(fps_history.begin());
	}
}
void Timer::setCorrection(float c)
{
	m_clocksPerSecond = c;
}
double Timer::getDelta(unsigned short d)
{
	if (d > TIMER_MAXSTAMPS - 1)
		d = TIMER_MAXSTAMPS - 1;
	return delta[d];
}
double Timer::getLocalDelta(unsigned short d)
{
	if (d > TIMER_MAXSTAMPS - 1)
		d = TIMER_MAXSTAMPS - 1;

	unsigned short d_prev = 0;

	if (d == TIMER_FRAME_GLOBAL)
		d_prev = m_stampsCount;
	else
		d_prev = d - 1;
	return abs((long)(m_currentClock[d] - m_currentClock[d_prev])) / m_clocksPerSecond;
}
int Timer::getFramesCount()
{
	return m_framesCount;
}
float Timer::getFPSStamp()
{
	return m_currentFPS;
}
Timer::Timer()
{
	m_stampsCount = 0;
	for (unsigned int i = 0; i < 100; i++)
	{
		fps_history.push_back(0);
	}
	m_clocksPerSecond = CLOCKS_PER_SEC;
	minFps = -1;
	maxFps = -1;
}

namespace Antimony
{
	Timer timer;
}
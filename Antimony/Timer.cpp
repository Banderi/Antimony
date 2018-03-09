#include <ctime>				// required for clock() and CLOCKS_PER_SEC					(Timer::UpdateDelta; Timer::GetFPSStamp)
#include <Shlwapi.h>			// required for abs()										(Timer::UpdateDelta)

#include "Timer.h"

///

void Timer::catchTime(int d)
{
	if (d == TIMER_FRAME_GLOBAL)
	{
		catchTime(TIMER_END);
		for (int i = 0; i < m_currentClock.size(); i++)
		{
			m_previousClock.at(i) = m_currentClock.at(i);
		}
	}

	m_currentClock.at(d) = clock();

	static clock_t clockLastSecond = m_currentClock.at(d) - m_clocksPerSecond;

	if (d == TIMER_END)
	{
		float den = m_derivClock.at(0);
		for (int i = 0; i < m_derivClock.size(); i++)
		{
			m_derivClock.at(i) -= den;
		}
	}

	if (d == TIMER_FRAME_GLOBAL)
	{
		double delta = getDelta();

		float sm = 5.0f * delta;
		lastDelta += (delta - lastDelta) * sm;
		for (int i = 0; i < m_derivClock.size(); i++)
		{
			m_derivClock.at(i) += (double(m_previousClock.at(i)) - m_derivClock.at(i)) * sm;
		}

		///

		double stamp = delta * 1000;

		if (stamp < 0)
			stamp = 0;
		if (stamp > 100)
			stamp = 100;
		fps_history.push_back(stamp);
		if (fps_history.size() > 100)
			fps_history.erase(fps_history.begin());

		///

		m_framesCount++;

		if (m_currentClock.at(d) >= clockLastSecond + m_clocksPerSecond)
		{
			m_framesCount = 0;
			clockLastSecond += m_clocksPerSecond;
		}

		static double clockLastStamp = m_currentClock.at(d) - 200;
		static int frames200Stamp = 0;
		frames200Stamp++;

		if (m_currentClock.at(d) >= clockLastStamp + 200)
		{
			auto frameAverageTime = frames200Stamp / (m_currentClock.at(d) - clockLastStamp);
			m_currentFPS = m_clocksPerSecond * frameAverageTime;

			if (maxFps < m_currentFPS)
				maxFps = m_currentFPS;
			if (minFps > m_currentFPS)
				minFps = m_currentFPS;
			if (minFps == -1)
				minFps = 60;

			clockLastStamp = m_currentClock.at(d);
			frames200Stamp = 0;
		}
	}
}
void Timer::setCorrection(float c)
{
	m_clocksPerSecond = c;
}

double Timer::getStep(int d1, int d2, bool smooth)
{
	if (smooth)
		return abs(double(m_derivClock.at(d1) - m_derivClock.at(d2))) / double(m_clocksPerSecond);
	else
		return abs(double(m_previousClock.at(d1) - m_previousClock.at(d2))) / double(m_clocksPerSecond);
}
double Timer::getDelta(int d1, int d2)
{
	return abs(double(m_currentClock.at(d1) - m_previousClock.at(d2))) / double(m_clocksPerSecond);
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
	for (unsigned int i = 0; i < TIMER_MAXSTAMPS; i++)
	{
		m_currentClock.push_back(0);
		m_previousClock.push_back(0);
		m_derivClock.push_back(0);
	}
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
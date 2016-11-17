#include <ctime>

#include "Shlwapi.h"

#include "Timer.h"

//

void Timer::UpdateDelta(char d)
{
	m_previousClock[d] = m_currentClock[d];
	m_currentClock[d] = clock();
	m_delta[d] = abs((long)(m_currentClock[d] - m_previousClock[d])) / m_correction;

	if (d == TIMER_FRAME_GLOBAL)
		m_framesCount++;

	if (m_currentClock[d] >= m_clockStamp + m_correction)
	{
		m_fpsStamp = (float)m_framesCount - (float)((m_currentClock[d] - (m_clockStamp + m_correction)) / (m_currentClock[d] - m_previousClock[d]));
		m_framesCount = 0;
		m_clockStamp += m_correction;

		if (maxFps < m_fpsStamp)
			maxFps = m_fpsStamp;
		if (minFps > m_fpsStamp)
			minFps = m_fpsStamp;
		if (minFps == -1)
			minFps = 60;
	}
}
void Timer::SetCorrection(float c)
{
	m_correction = c;
}
double Timer::GetDelta(char d)
{
	return m_delta[d];
}
int Timer::GetFramesCount()
{
	return m_framesCount;
}
float Timer::GetFPSStamp()
{
	return m_fpsStamp;
}
Timer::Timer()
{
	m_correction = CLOCKS_PER_SEC;
	minFps = -1;
	maxFps = -1;
}

Timer timer;
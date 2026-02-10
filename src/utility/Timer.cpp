#include "utility/Timer.h"


Timer::Timer()
{
	m_StartTime = std::chrono::high_resolution_clock::now();
}


double Timer::GetElapsed()
{
	auto now = std::chrono::high_resolution_clock::now();
	auto elapsedDuration = now - m_StartTime;

	return elapsedDuration.count() / 1000000000.0;
}


void Timer::Restart()
{
	m_StartTime = std::chrono::high_resolution_clock::now();
}
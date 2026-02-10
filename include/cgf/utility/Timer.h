#pragma once

#include <chrono>


class Timer
{
public:
	Timer();

	double GetElapsed();
	
	void Restart();

private:
	std::chrono::high_resolution_clock::time_point m_StartTime;
};
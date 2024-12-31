#include <windows.h>
#include "Timer.h"

TimerType::TimerType()
{
	QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&ticksPerSecond));
	secondsPerTick = 1.0 / static_cast<double>(ticksPerSecond);
	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&previousTime));

	deltaTime = 0.0;
	framesPerSecond = 0;
	currentTime = 0;
}

void TimerType::CheckTime()
{
	__int64 currTime;

	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&currTime));
	currentTime = currTime;

	// Time difference between this frame and the previous in seconds
	deltaTime = (currentTime - previousTime) * secondsPerTick;

	framesPerSecond = static_cast<int>(ticksPerSecond / (currentTime - previousTime));

	// Prepare for next frame
	previousTime = currentTime;

	// Force nonnegative
	if (deltaTime < 0.0) deltaTime = 0.0;
}

double TimerType::GetTimeDeltaTime() const
{
	return deltaTime;
}
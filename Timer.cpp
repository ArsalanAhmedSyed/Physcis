#include "Timer.h"

Timer::Timer()
{
	_lastFrame = steady_clock::now();
}

void Timer::Tick()
{
	_lastFrame = steady_clock::now();
}

float Timer::GetDeltaTime()
{
	return duration<float>(steady_clock::now() - _lastFrame).count();
}

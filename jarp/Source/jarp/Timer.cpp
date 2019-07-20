#include "jarppch.h"
#include "Timer.h"

#include "SDL.h"

Timer::Timer()
{
	CurrentFrameTime = Timer::GetTime();
	LastFrameTime = CurrentFrameTime;
	DeltaFrameTime = 0;

	CurrentFPSTime = Timer::GetTime();
	LastFPSTime = CurrentFPSTime;
	FramesPerSecondCount = 0;
}

void Timer::Update()
{
	CurrentFrameTime = Timer::GetTime();
	DeltaFrameTime = CurrentFrameTime - LastFrameTime;
	LastFrameTime = CurrentFrameTime;

	CurrentFPSTime = Timer::GetTime();
	++FramesPerSecondCount;
	if (CurrentFPSTime > LastFPSTime + 1000)
	{
		JARP_CORE_TRACE("{0} fps", FramesPerSecondCount);
		LastFPSTime = CurrentFPSTime;
		FramesPerSecondCount = 0;
	}
}

unsigned int Timer::GetTime()
{
	return SDL_GetTicks();
}

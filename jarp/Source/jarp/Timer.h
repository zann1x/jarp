#pragma once

class Timer
{
public:	
	Timer();

	void Update();
	inline uint32_t GetTimeSinceLastFrame() const { return DeltaFrameTime; }

	static uint32_t GetTime();

private:
	uint32_t CurrentFrameTime;
	uint32_t LastFrameTime;
	uint32_t DeltaFrameTime;

	uint32_t CurrentFPSTime;
	uint32_t LastFPSTime;
	uint32_t FramesPerSecondCount;
};

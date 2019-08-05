#pragma once

#include "SDL.h"

class Time
{
public:
	static uint32_t GetTicks()
	{
		return SDL_GetTicks();
	}
};

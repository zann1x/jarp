#pragma once

#include "SDL.h"

class Time
{
public:

	/* Get the number of milliseconds since the SDL library initialization. */
	static uint32_t GetTicks()
	{
		return SDL_GetTicks();
	}
};

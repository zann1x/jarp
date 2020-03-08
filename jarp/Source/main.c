#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include <SDL.h>

#include "buttons.h"
#include "input.h"
#include "keys.h"
#include "log.h"
#include "window.h"

int main(int argc, char** argv)
{
	log_trace("Tracing... %s", "asdf");
	log_debug("Debugging...");
	log_info("Infoing...");
	log_warn("Warning...");
	log_error("Erroring...");
	log_fatal("Fataling...");

	window_init();

	uint32_t currentFPSTime = SDL_GetTicks();
	uint32_t lastFPSTime = currentFPSTime;
	uint32_t frames = 0;

	bool isRunning = true;
	while (isRunning)
	{
		currentFPSTime = SDL_GetTicks();
		++frames;
		if (currentFPSTime > lastFPSTime + 1000)
		{
			printf("fps: %d\n", frames);
			lastFPSTime = currentFPSTime;
			frames = 0;
		}

		// Handle events
		SDL_Event event;
		while (SDL_PollEvent(&event) != 0)
		{
			switch (event.type)
			{
				case SDL_QUIT:
				{
					isRunning = false;
					break;
				}
				case SDL_KEYDOWN:
				case SDL_KEYUP:
				case SDL_MOUSEBUTTONDOWN:
				case SDL_MOUSEBUTTONUP:
				case SDL_MOUSEMOTION:
				case SDL_MOUSEWHEEL:
				{
					input_event(&event);
					break;
				}
				case SDL_WINDOWEVENT:
				{
					window_event(&event);
					break;
				}
			}
		}

		// render and update stuff
		input_update();
	}

	window_destroy();

	return 0;
}

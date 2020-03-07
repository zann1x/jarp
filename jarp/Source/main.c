#include <stdio.h>
#include <stdbool.h>

#include <SDL.h>

#include "log.h"

int main(int argc, char** argv)
{
	log_trace("Tracing... %s", "asdf");
	log_debug("Debugging...");
	log_info("Infoing...");
	log_warn("Warning...");
	log_error("Erroring...");
	log_fatal("Fataling...");

	// Initialize SDL
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
	SDL_Window* window = SDL_CreateWindow("Hello world", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);
	SDL_SetWindowResizable(window, true);

	uint32_t currentFPSTime = SDL_GetTicks();
	uint32_t lastFPSTime = currentFPSTime;
	uint32_t frames = 0;

	bool isRunning = true;
	while (isRunning == true)
	{
		currentFPSTime = SDL_GetTicks();
		++frames;
		if (currentFPSTime > lastFPSTime + 1000)
		{
			printf("%d fps\n", frames);
			lastFPSTime = currentFPSTime;
			frames = 0;
		}

		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_QUIT:
				isRunning = false;
				break;
			}
		}

		SDL_Surface* surface = SDL_GetWindowSurface(window);
		SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0xff, 0xdd, 0xaa));
		SDL_UpdateWindowSurface(window);
	}

	// Quit SDL
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}

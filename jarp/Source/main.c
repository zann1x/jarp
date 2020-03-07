#include <stdio.h>
#include <stdbool.h>

#include <SDL.h>

#include "log.h"

struct GameInput
{
	bool move_up;

	int mouse_x;
	int mouse_y;
	bool mouse_left;
	bool mouse_right;
} input;

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

	SDL_Surface* surface = SDL_GetWindowSurface(window);
	SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0xff, 0xdd, 0xaa));
	SDL_UpdateWindowSurface(window);

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
				isRunning = false;
				break;
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym)
				{
				case SDLK_e:
					log_info("Pressed E");
					break;
				}
				break;
			case SDL_MOUSEMOTION:
				SDL_GetMouseState(&input.mouse_x, &input.mouse_y);
				log_info("x: %d, y: %d", input.mouse_x, input.mouse_y);
				break;
			}
		}

		uint32_t mouse_button = SDL_GetMouseState(NULL, NULL);
		input.mouse_left = mouse_button == SDL_BUTTON_LMASK;
		if (mouse_button == SDL_BUTTON_RMASK)
			input.mouse_right = mouse_button == SDL_BUTTON_RMASK;

		// Handle input
		const uint8_t* key_states = SDL_GetKeyboardState(NULL);
		input.move_up = key_states[SDL_SCANCODE_W];

		// render and update stuff
		if (input.mouse_left)
			log_info("left mouse button pressed");
		if (input.mouse_right)
			log_info("right mouse button pressed");
	}

	// Quit SDL
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}

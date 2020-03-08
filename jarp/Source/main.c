#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <SDL.h>

#include "input/buttons.h"
#include "input/input.h"
#include "input/keys.h"
#include "file.h"
#include "log.h"
#include "window.h"

static bool is_running;

void handle_events()
{
	SDL_Event event;
	while (SDL_PollEvent(&event) != 0)
	{
		switch (event.type)
		{
			case SDL_QUIT:
			{
				is_running = false;
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
}

int main(int argc, char** argv)
{
	log_trace("Tracing... %s", "asdf");
	log_debug("Debugging...");
	log_info("Infoing...");
	log_warn("Warning...");
	log_error("Erroring...");
	log_fatal("Fataling...");

	char* buffer = file_read_asc("E:\\VisualStudioProjects\\jarp\\jarp\\Source\\main.c");
	if (buffer != NULL)
	{
		log_info("%s", buffer);
		free(buffer);
	}

	window_init();

	uint32_t current_fps_time = SDL_GetTicks();
	uint32_t last_fps_time = current_fps_time;
	uint32_t frames = 0;

	is_running = true;
	while (is_running)
	{
		current_fps_time = SDL_GetTicks();
		++frames;
		if (current_fps_time > last_fps_time + 1000)
		{
			printf("fps: %d\n", frames);
			last_fps_time = current_fps_time;
			frames = 0;
		}

		handle_events();

		// render and update stuff
		input_update();
	}

	window_destroy();

	return EXIT_SUCCESS;
}

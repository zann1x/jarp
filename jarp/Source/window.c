#include "window.h"

#include <stdbool.h>

#include "log.h"

struct NativeWindow
{
	SDL_Window* window;
	SDL_Surface* surface;
} native_window;

void window_init()
{
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
	native_window.window = SDL_CreateWindow("Hello world", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);
	SDL_SetWindowResizable(native_window.window, true);

	native_window.surface = SDL_GetWindowSurface(native_window.window);
	SDL_FillRect(native_window.surface, NULL, SDL_MapRGB(native_window.surface->format, 0xff, 0xdd, 0xaa));
	SDL_UpdateWindowSurface(native_window.window);
}

void window_destroy()
{
	SDL_DestroyWindow(native_window.window);
	SDL_Quit();
}

void window_event(SDL_Event* event)
{
	switch (event->window.event)
	{
		case SDL_WINDOWEVENT_SIZE_CHANGED:
		case SDL_WINDOWEVENT_RESIZED:
		{
			window.width = event->window.data1;
			window.height = event->window.data2;
			log_trace("Window resized to (%d, %d)", event->window.data1, event->window.data2);
			break;
		}
		default:
		{
			log_warn("Unknown window event type %d", event->window.event);
			break;
		}
	}
}

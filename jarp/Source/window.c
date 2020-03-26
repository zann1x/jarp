#include "window.h"

#include <stdbool.h>

#include "log.h"

struct Window window;

void window_init(void)
{
    window.width = 800;
    window.height = 600;
    window.title = "jarp";

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
    window.handle= SDL_CreateWindow(window.title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window.width, window.height, SDL_WINDOW_SHOWN);
    SDL_SetWindowResizable(window.handle, true);

    window.surface = SDL_GetWindowSurface(window.handle);
    SDL_FillRect(window.surface, NULL, SDL_MapRGB(window.surface->format, 0xff, 0xdd, 0xaa));
    SDL_UpdateWindowSurface(window.handle);
}

void window_destroy(void)
{
    SDL_DestroyWindow(window.handle);
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

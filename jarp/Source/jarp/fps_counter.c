#include "fps_counter.h"

#include <stdio.h>

#include <SDL.h>

#include "api_types.h"
#include "log.h"
#include "window.h"

uint32_t current_fps_time;
uint32_t last_fps_time;
uint32_t frames;

void fps_counter_init(void)
{
    current_fps_time = SDL_GetTicks();
    last_fps_time = current_fps_time;
    frames = 0;
}

void fps_counter_update(const struct Window* window)
{
    current_fps_time = SDL_GetTicks();
    ++frames;
    if (current_fps_time > last_fps_time + 1000)
    {
        char buffer[32];
        sprintf(buffer, "%s - %d fps", window->title, frames);
        SDL_SetWindowTitle(window->handle, buffer);

        last_fps_time = current_fps_time;
        frames = 0;
    }
}


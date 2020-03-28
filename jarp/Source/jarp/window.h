#pragma once

#include "api_types.h"

struct Window
{
    int32_t width;
    int32_t height;
    const char* title;

    struct SDL_Window* handle;
    struct SDL_Surface* surface;
};

extern struct Window window;

void window_init(void);
void window_destroy(void);
void window_set_display_title(const char* title);

void window_event(union SDL_Event* event);

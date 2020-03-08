#pragma once

#include <stdint.h>

#include <SDL.h>

struct Window
{
	int32_t width;
	int32_t height;
};

extern struct Window window;

void window_init();
void window_destroy();

void window_event(SDL_Event* event);

#pragma once

#include <SDL.h>

#include "keys.h"

extern int input_mouse_x;
extern int input_mouse_y;
extern uint16_t input_key_down[KEY_COUNT];
extern uint8_t input_button_down[16];

void input_event(SDL_Event* event);
void input_update();

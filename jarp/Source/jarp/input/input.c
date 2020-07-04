#include "input.h"

#include <SDL.h>

#include "jarp/api_types.h"
#include "jarp/log.h"

int input_mouse_x = 0;
int input_mouse_y = 0;
uint16_t input_key_down[KEY_COUNT];
uint8_t input_button_down[16];

void input_event(SDL_Event *event) {
    switch (event->type) {
        case SDL_KEYDOWN: {
            if (event->key.repeat == 0) {
                log_trace("Key was pressed");
            } else {
                log_trace("Key was pressed repeatedly");
            }
            input_key_down[event->key.keysym.scancode] = true;
            break;
        }
        case SDL_KEYUP: {
            log_trace("Key was released");
            input_key_down[event->key.keysym.scancode] = false;
            break;
        }
        case SDL_MOUSEBUTTONDOWN: {
            log_trace("Mouse button was pressed");
            input_button_down[event->button.button] = true;
            break;
        }
        case SDL_MOUSEBUTTONUP: {
            log_trace("Mouse button was released");
            input_button_down[event->button.button] = false;
            break;
        }
        case SDL_MOUSEMOTION: {
            log_trace("Mouse moved (%d, %d) and is now at (%d, %d)", event->motion.xrel, event->motion.yrel, input_mouse_x, input_mouse_x);
            break;
        }
        case SDL_MOUSEWHEEL: {
            log_trace("Mouse wheel moved (%d, %d)", event->wheel.x, event->wheel.y);
            break;
        }
        default: {
            log_warn("Unhandled input event type %d", event->type);
            break;
        }
    }
}

void input_update(void) {
    if (SDL_GetRelativeMouseMode() == true) {
        SDL_GetRelativeMouseState(&input_mouse_x, &input_mouse_y);
    } else {
        SDL_GetMouseState(&input_mouse_x, &input_mouse_y);
    }
}

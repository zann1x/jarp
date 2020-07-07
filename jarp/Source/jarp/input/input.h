#ifndef INPUT_H
#define INPUT_H

enum EButton {
    BUTTON_LEFT = SDL_BUTTON_LEFT,
    BUTTON_MIDDLE = SDL_BUTTON_MIDDLE,
    BUTTON_RIGHT = SDL_BUTTON_RIGHT,
    BUTTON_BACK = SDL_BUTTON_X1,
    BUTTON_FORWARD = SDL_BUTTON_X2
};

enum EKey {
    KEY_UNKNOWN = SDL_SCANCODE_UNKNOWN,

    KEY_W = SDL_SCANCODE_W,
    KEY_A = SDL_SCANCODE_A,
    KEY_S = SDL_SCANCODE_S,
    KEY_D = SDL_SCANCODE_D,
    KEY_SPACE = SDL_SCANCODE_SPACE,

    KEY_LEFT = SDL_SCANCODE_LEFT,
    KEY_RIGHT = SDL_SCANCODE_RIGHT,

    KEY_COUNT = SDL_NUM_SCANCODES
};

extern int input_mouse_x;
extern int input_mouse_y;
extern uint16_t input_key_down[KEY_COUNT];
extern uint8_t input_button_down[16];

void input_event(union SDL_Event* event);
void input_update();

#endif

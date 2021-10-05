#define SDL_MAIN_HANDLED

#include <SDL.h>
#include <SDL_syswm.h>
#include <glad/glad.h>
#include "Renderer.h"
#include "Win32Window.h"

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG
#include <spdlog/spdlog.h>

Win32Window win32_window;
Renderer renderer;

enum EButton {
    JARP_BUTTON_UNKNOWN = 0,
    JARP_BUTTON_LEFT,
    JARP_BUTTON_MIDDLE,
    JARP_BUTTON_RIGHT,
    JARP_BUTTON_BACK,
    JARP_BUTTON_FORWARD,

    JARP_BUTTON_COUNT = 512,
};

enum EKey {
    JARP_KEY_UNKNOWN = 0,

    JARP_KEY_A,
    JARP_KEY_D,
    JARP_KEY_S,
    JARP_KEY_W,
    JARP_KEY_SPACE,
    JARP_KEY_LEFT,
    JARP_KEY_RIGHT,

    JARP_KEY_COUNT = 512,
};

bool is_running = false;
int win32_input_mouse_x = 0;
int win32_input_mouse_y = 0;
static uint16_t win32_input_key_down[JARP_KEY_COUNT];
static uint8_t win32_input_button_down[JARP_BUTTON_COUNT];

enum EKey window_input_remap_key(SDL_Scancode scancode) {
    switch (scancode) {
        case SDL_SCANCODE_UNKNOWN:
            return JARP_KEY_UNKNOWN;
        case SDL_SCANCODE_A:
            return JARP_KEY_A;
        case SDL_SCANCODE_D:
            return JARP_KEY_D;
        case SDL_SCANCODE_S:
            return JARP_KEY_S;
        case SDL_SCANCODE_W:
            return JARP_KEY_W;
        case SDL_SCANCODE_SPACE:
            return JARP_KEY_SPACE;
        default:
            return JARP_KEY_UNKNOWN;
    }
}

void handle_events() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT: {
                is_running = false;
                break;
            }
            case SDL_KEYDOWN: {
                if (event.key.repeat == 0) {
                    enum EKey key = window_input_remap_key(event.key.keysym.scancode);
                    win32_input_key_down[key] = true;
                }
                break;
            }
            case SDL_KEYUP: {
                enum EKey key = window_input_remap_key(event.key.keysym.scancode);
                win32_input_key_down[key] = false;
                break;
            }
            case SDL_MOUSEBUTTONDOWN: {
                win32_input_button_down[event.button.button] = true;
                break;
            }
            case SDL_MOUSEBUTTONUP: {
                win32_input_button_down[event.button.button] = false;
                break;
            }
//                case SDL_MOUSEMOTION: {
//                    // Movement: (event.motion.xrel, event.motion.yrel)
//                    // Position: (win32_input_mouse_x, win32_input_mouse_y)
//                    break;
//                }
//                case SDL_MOUSEWHEEL: {
//                    // Movement: (event.wheel.x, event.wheel.y)
//                    break;
//                }
            case SDL_WINDOWEVENT: {
                switch (event.window.event) {
                    case SDL_WINDOWEVENT_SIZE_CHANGED:
                    case SDL_WINDOWEVENT_RESIZED: {
                        win32_window.width = event.window.data1;
                        win32_window.height = event.window.data2;
                        win32_window.is_resized = true;
                        break;
                    }
                    case SDL_WINDOWEVENT_MINIMIZED: {
                        win32_window.is_minimized = true;
                        break;
                    }
                    case SDL_WINDOWEVENT_RESTORED: {
                        win32_window.is_minimized = false;
                        break;
                    }
                }
                break;
            }
        }
    }
    if (SDL_GetRelativeMouseMode() == SDL_TRUE) {
        SDL_GetRelativeMouseState(&win32_input_mouse_x, &win32_input_mouse_y);
    } else {
        SDL_GetMouseState(&win32_input_mouse_x, &win32_input_mouse_y);
    }
}

int main() {
    win32_window.init();
    renderer.init();
    spdlog::set_level(spdlog::level::trace);

    uint32_t current_fps_time = SDL_GetTicks();
    uint32_t last_fps_time = current_fps_time;
    uint32_t frames = 0;
    double delta_ms = 0.0;

    is_running = true;
    while (is_running) {
        handle_events();

        win32_window.clear();
        renderer.draw(delta_ms);
        win32_window.swap();

        current_fps_time = SDL_GetTicks();
        delta_ms = (double) (current_fps_time - last_fps_time) / 1000;
        ++frames;
        if (delta_ms > 1.0) {
            SPDLOG_TRACE("{:d} fps", frames);
            last_fps_time = current_fps_time;
            frames = 0;
        }
    }
    win32_window.shutdown();

    return 0;
}

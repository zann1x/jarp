#define SDL_MAIN_HANDLED

#include <SDL.h>
#include <SDL_syswm.h>
#include <glad/glad.h>
#include "renderer.h"
#include "log.h"

struct Win32Window {
    int width;
    int height;
    const char *title;
    bool is_minimized;
    bool is_resized;

    struct SDL_Window *handle;

    SDL_GLContext gl_context;
    SDL_SysWMinfo system_info;
} win32_window;

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

void window_init() {
    win32_window.width = 800;
    win32_window.height = 600;
    win32_window.title = "jarp";
    win32_window.is_minimized = false;

    SDL_Init(SDL_INIT_EVERYTHING);
//    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
//    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
//    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetSwapInterval(0);
    win32_window.handle = SDL_CreateWindow(win32_window.title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                           win32_window.width, win32_window.height,
                                           SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    win32_window.gl_context = SDL_GL_CreateContext(win32_window.handle);

    SDL_VERSION(&win32_window.system_info.version)
    SDL_GetWindowWMInfo(win32_window.handle, &win32_window.system_info);

    gladLoadGLLoader((GLADloadproc) SDL_GL_GetProcAddress);

    glClearColor(0.f, 0.f, 0.f, 1.f);
}

void window_clear() {
    glClear(GL_COLOR_BUFFER_BIT);
}

void window_swap() {
    SDL_GL_SwapWindow(win32_window.handle);
}

void window_shutdown() {
    SDL_GL_DeleteContext(win32_window.gl_context);
    SDL_DestroyWindow(win32_window.handle);
    SDL_Quit();
}

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
    log_set_level(LOG_LEVEL_TRACE);
    window_init();
    renderer_init();

    uint32_t current_fps_time = SDL_GetTicks();
    uint32_t last_fps_time = current_fps_time;
    uint32_t frames = 0;
    double delta_ms = 0.0;

    is_running = true;
    while (is_running) {
        handle_events();

        window_clear();
        renderer_draw(delta_ms);
        window_swap();

        current_fps_time = SDL_GetTicks();
        delta_ms = (double) (current_fps_time - last_fps_time) / 1000;
        ++frames;
        if (delta_ms > 1.0) {
            log_info("%d fps", frames);
            last_fps_time = current_fps_time;
            frames = 0;
        }
    }
    window_shutdown();

    return 0;
}

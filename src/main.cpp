#define SDL_MAIN_HANDLED
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include <SDL.h>
#include <SDL_syswm.h>
#include <glad/glad.h>
#include <spdlog/spdlog.h>
#include <array>
#include <chrono>
#include "Renderer.h"
#include "Win32Window.h"

Win32Window win32_window;

bool is_running = false;
int win32_input_mouse_x = 0;
int win32_input_mouse_y = 0;
std::array<bool, SDL_NUM_SCANCODES> win32_input_key_down;
std::array<bool, SDL_MAX_UINT8> win32_input_button_down;

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
                    win32_input_key_down.at(event.key.keysym.scancode) = true;
                }
                break;
            }
            case SDL_KEYUP: {
                win32_input_key_down.at(event.key.keysym.scancode) = false;
                break;
            }
            case SDL_MOUSEBUTTONDOWN: {
                win32_input_button_down.at(event.button.button) = true;
                break;
            }
            case SDL_MOUSEBUTTONUP: {
                win32_input_button_down.at(event.button.button) = false;
                break;
            }
            case SDL_MOUSEMOTION: {
                // Movement: (event.motion.xrel, event.motion.yrel)
                // Position: (win32_input_mouse_x, win32_input_mouse_y)
                break;
            }
            case SDL_MOUSEWHEEL: {
                // Movement: (event.wheel.x, event.wheel.y)
                break;
            }
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
    spdlog::set_level(spdlog::level::trace);

    win32_window.create_and_load_gl();
    Renderer renderer;
    renderer.load_sample_render_data();

    auto current_fps_time = std::chrono::high_resolution_clock::now();
    auto last_fps_time = current_fps_time;
    uint32_t frames = 0;
    double delta_ms = 0.0;

    is_running = true;
    while (is_running) {
        handle_events();

        if (!win32_window.is_minimized) {
            renderer.draw(delta_ms);
        }
        win32_window.swap();

        current_fps_time = std::chrono::high_resolution_clock::now();
        auto time_diff = std::chrono::duration_cast<std::chrono::milliseconds>(current_fps_time - last_fps_time);
        delta_ms = (double) (time_diff.count()) / 1000;
        ++frames;
        if (delta_ms > 1.0) {
            SPDLOG_TRACE("{:d} fps", frames);
            last_fps_time = current_fps_time;
            frames = 0;
        }
    }

    return 0;
}

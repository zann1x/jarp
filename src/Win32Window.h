#ifndef WIN32_WINDOW_H
#define WIN32_WINDOW_H

#include <SDL.h>
#include <SDL_syswm.h>

class Win32Window {
public:
    ~Win32Window();

    void create_and_load_gl();
    void swap();

    int width{ 800 };
    int height{ 600 };
    const char* title{ "jarp" };
    bool is_minimized{ false };
    bool is_resized{ false };

private:
    SDL_Window* handle{ nullptr };

    SDL_GLContext gl_context{ nullptr };
    SDL_SysWMinfo system_info{};
};

#endif WIN32_WINDOW_H

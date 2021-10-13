#ifndef WIN32_WINDOW_H
#define WIN32_WINDOW_H

#include <SDL.h>
#include <imgui.h>

class Win32Window {
public:
    Win32Window();
    ~Win32Window();

    void swap();

    int width{ 1600 };
    int height{ 900 };
    const char* title{ "jarp" };
    bool is_minimized{ false };
    bool is_resized{ false };

private:
    SDL_Window* handle{ nullptr };
    SDL_GLContext gl_context{ nullptr };
    ImGuiContext* imgui_context{ nullptr };
};

#endif

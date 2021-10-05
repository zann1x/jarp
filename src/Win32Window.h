#ifndef WIN32_WINDOW_H
#define WIN32_WINDOW_H

#include "Window.h"
//#include <SDL.h>
#include <SDL_syswm.h>

class Win32Window : public Window {
public:
    void init();
    void shutdown();
    void clear();
    void swap();

    int width;
    int height;
    const char* title;
    bool is_minimized;
    bool is_resized;

private:
    SDL_Window* handle;

    SDL_GLContext gl_context;
    SDL_SysWMinfo system_info;
};

#endif WIN32_WINDOW_H

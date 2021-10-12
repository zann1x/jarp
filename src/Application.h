#ifndef APPLICATION_H
#define APPLICATION_H

#include "Renderer.h"
#include "Win32Window.h"

class Application {
public:
    void run();

private:
    void handle_events();

private:
    Win32Window win32_window;
    Renderer renderer;

    bool is_running = false;
};

#endif 

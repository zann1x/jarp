#ifndef APPLICATION_H
#define APPLICATION_H

#include "Camera.h"
#include "Renderer.h"
#include "Win32Window.h"

class Application {
public:
    Application();
    void run();

private:
    void handle_events();
    void on_window_close();

private:
    Win32Window win32_window;
    Renderer renderer;
    Camera camera;

    bool is_running = false;
};

#endif 

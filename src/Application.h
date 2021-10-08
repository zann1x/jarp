#ifndef APPLICATION_H
#define APPLICATION_H

#include "Win32Window.h"

class Application {
public:
    Application();
    ~Application();

    void run();

private:
    void handle_events();

    Win32Window win32_window;
    bool is_running = false;
};

#endif 

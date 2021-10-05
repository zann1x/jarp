#include "Win32Window.h"
#include <SDL.h>
#include <glad/glad.h>
#include <spdlog/spdlog.h>

Win32Window::Win32Window() {
    SDL_Init(SDL_INIT_EVERYTHING);
    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetSwapInterval(0);
}

void Win32Window::create_and_load_gl() {
    this->handle = SDL_CreateWindow(this->title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        this->width, this->height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    this->gl_context = SDL_GL_CreateContext(this->handle);

    SDL_VERSION(&this->system_info.version);
    SDL_GetWindowWMInfo(this->handle, &this->system_info);

    gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress);
    if (!gladLoadGL()) {
        throw std::runtime_error("Failed to initialize OpenGL context");
    }
    SPDLOG_INFO("OpenGL {:d}.{:d}", GLVersion.major, GLVersion.minor);
}

Win32Window::~Win32Window() {
    SDL_GL_DeleteContext(this->gl_context);
    SDL_DestroyWindow(this->handle);
    SDL_Quit();
}

void Win32Window::swap() {
    SDL_GL_SwapWindow(this->handle);
}

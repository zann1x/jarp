#include "Win32Window.h"
#include <glad/glad.h>
#include <spdlog/spdlog.h>

Win32Window::Win32Window() {
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetSwapInterval(0);

    this->handle = SDL_CreateWindow(this->title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        this->width, this->height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);
    this->gl_context = SDL_GL_CreateContext(this->handle);

    gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress);
    if (!gladLoadGL()) {
        throw std::runtime_error("Failed to load OpenGL");
    }
}

Win32Window::~Win32Window() {
    SDL_GL_DeleteContext(this->gl_context);
    SDL_DestroyWindow(this->handle);
}

void Win32Window::swap() {
    SDL_GL_SwapWindow(this->handle);
}

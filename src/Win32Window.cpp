#include "Win32Window.h"
#include <glad/glad.h>
#include <spdlog/spdlog.h>

Win32Window::Win32Window() {
    this->handle = SDL_CreateWindow(this->title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        this->width, this->height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    this->gl_context = SDL_GL_CreateContext(this->handle);
}

Win32Window::~Win32Window() {
    SDL_GL_DeleteContext(this->gl_context);
    SDL_DestroyWindow(this->handle);
}

void Win32Window::swap() {
    SDL_GL_SwapWindow(this->handle);
}

void Win32Window::make_current() {
    SDL_GL_MakeCurrent(this->handle, this->gl_context);
}

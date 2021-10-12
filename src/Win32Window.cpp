#include "Win32Window.h"
#include <glad/glad.h>
#include <spdlog/spdlog.h>

#include <backends/imgui_impl_sdl.h>
#include <backends/imgui_impl_opengl3.h>

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

    this->imgui_context = ImGui::CreateContext();
    ImGui_ImplSDL2_InitForOpenGL(this->handle, this->gl_context);
    ImGui_ImplOpenGL3_Init("#version 460");
}

Win32Window::~Win32Window() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext(this->imgui_context);

    SDL_GL_DeleteContext(this->gl_context);
    SDL_DestroyWindow(this->handle);
}

void Win32Window::swap() {
    SDL_GL_SwapWindow(this->handle);
}

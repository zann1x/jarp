#include "Win32Window.h"
#include <SDL.h>
#include <glad/glad.h>

void Win32Window::init()
{
    width = 800;
    height = 600;
    title = "jarp";
    is_minimized = false;

    SDL_Init(SDL_INIT_EVERYTHING);
    //    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    //    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    //    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetSwapInterval(0);
    handle = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        width, height,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    gl_context = SDL_GL_CreateContext(handle);

    SDL_VERSION(&system_info.version);
    SDL_GetWindowWMInfo(handle, &system_info);

    gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress);

    glClearColor(0.f, 0.f, 0.f, 1.f);
}

void Win32Window::shutdown()
{
    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(handle);
    SDL_Quit();
}

void Win32Window::clear()
{
    glClear(GL_COLOR_BUFFER_BIT);
}

void Win32Window::swap()
{
    SDL_GL_SwapWindow(handle);
}

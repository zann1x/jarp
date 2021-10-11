#define SDL_MAIN_HANDLED
#include <cstdlib>
#include <ctime>
#include <spdlog/spdlog.h>

#include "Application.h"

int main() {
    spdlog::set_level(spdlog::level::trace);
    std::srand(std::time(nullptr));

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        spdlog::error("SDL initialization failed: {:s}", SDL_GetError());
    }
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetSwapInterval(0);

    Application application;
    application.run();

    SDL_Quit();

    return 0;
}

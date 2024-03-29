#define SDL_MAIN_HANDLED
//#define GLM_FORCE_LEFT_HANDED
//#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <cstdlib>
#include <ctime>

#include <spdlog/spdlog.h>
#include <SDL.h>
#include <glad/glad.h>

#include "Application.h"
#include "VersionConfig.h"
#include "Win32Window.h"

int main() {
    spdlog::set_level(spdlog::level::trace);
    spdlog::info("{:s} Version {:s}", PROJECT_NAME, PROJECT_VERSION);
    std::srand(std::time(nullptr));

    try {
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
            throw std::runtime_error(SDL_GetError());
        }

        Application application;
        application.run();
    }
    catch (const std::exception &exception) {
        spdlog::critical("{:s}", exception.what());
    }

    SDL_Quit();

    return 0;
}

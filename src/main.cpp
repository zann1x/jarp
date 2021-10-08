#define SDL_MAIN_HANDLED
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include <spdlog/spdlog.h>
#include <cstdlib>
#include <ctime>

#include "Application.h"

int main() {
    spdlog::set_level(spdlog::level::trace);
    std::srand(std::time(nullptr));

    Application application;
    application.run();

    return 0;
}

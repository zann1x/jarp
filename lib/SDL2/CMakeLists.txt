message(STATUS "Fetching SDL...")

FetchContent_Declare(
    SDL2
    GIT_REPOSITORY "https://github.com/libsdl-org/SDL"
    GIT_TAG "release-2.0.14"
)

FetchContent_MakeAvailable(SDL2)

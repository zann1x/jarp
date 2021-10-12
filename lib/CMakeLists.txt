#[[
If modifications to the fetched library are necessary, refer to this video:
https://youtu.be/y9kSr5enrSk?t=2903
]]#
include(FetchContent)

message(STATUS "Fetching third party libraries")

add_subdirectory(glad)

FetchContent_Declare(
    glm
    GIT_REPOSITORY "https://github.com/g-truc/glm"
    GIT_TAG 0.9.9.8
)
add_subdirectory(glm)

add_subdirectory(imgui)

FetchContent_Declare(
    SDL2
    GIT_REPOSITORY "https://github.com/libsdl-org/SDL"
    GIT_TAG "release-2.0.14"
)
add_subdirectory(SDL2)

FetchContent_Declare(
    spdlog
    GIT_REPOSITORY "https://github.com/gabime/spdlog"
    GIT_TAG "v1.9.2"
)
add_subdirectory(spdlog)

message(STATUS "Done fetching third party libraries")

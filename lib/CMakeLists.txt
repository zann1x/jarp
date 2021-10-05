include(FetchContent)

message(STATUS "Fetching third party libraries")

add_subdirectory(glad)

FetchContent_Declare(
        glm
        GIT_REPOSITORY "https://github.com/g-truc/glm"
        GIT_TAG 0.9.9.8
)
add_subdirectory(glm)

FetchContent_Declare(
        SDL2
        GIT_REPOSITORY "https://github.com/libsdl-org/SDL"
        GIT_TAG "release-2.0.14"
)
add_subdirectory(SDL2)


message(STATUS "Done fetching third party libraries")

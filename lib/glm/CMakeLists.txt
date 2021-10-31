message(STATUS "Fetching glm...")

FetchContent_Declare(
    glm
    GIT_REPOSITORY "https://github.com/g-truc/glm"
    GIT_TAG "master"
)

FetchContent_MakeAvailable(glm)

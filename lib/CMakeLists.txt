include(FetchContent)

message(STATUS "Fetching third party libraries")

add_subdirectory(glad)
add_subdirectory(glm)
add_subdirectory(imgui)
add_subdirectory(SDL2)
add_subdirectory(spdlog)
add_subdirectory(stb)

message(STATUS "Done fetching third party libraries")

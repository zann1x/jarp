#[[
If modifications to the fetched library are necessary, refer to this video:
https://youtu.be/y9kSr5enrSk?t=2903
]]#
include(FetchContent)

message(STATUS "Fetching third party libraries")

add_subdirectory(glad)
add_subdirectory(glm)
add_subdirectory(imgui)
add_subdirectory(SDL2)
add_subdirectory(spdlog)

message(STATUS "Done fetching third party libraries")

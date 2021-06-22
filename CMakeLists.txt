cmake_minimum_required(VERSION 3.19)
project(jarp)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(lib)

add_executable(jarp src/main.cpp src/renderer.h src/renderer.cpp src/log.h src/log.cpp)
target_link_libraries(jarp PRIVATE SDL2 glad glm)

if(WIN32)
    target_link_libraries(jarp PRIVATE SDL2main)
    add_custom_command(TARGET jarp POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:SDL2>
            $<TARGET_FILE_DIR:jarp>)
endif()

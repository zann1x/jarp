cmake_minimum_required(VERSION 3.19)
project(jarp)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(lib)

add_executable(jarp src/main.cpp "src/Renderer.h" "src/Renderer.cpp" "src/Win32Window.cpp" "src/Win32Window.h" "src/FileSystem.h" "src/FileSystem.cpp" "src/Window.h")
target_link_libraries(jarp PRIVATE glad glm SDL2 spdlog)

if(WIN32)
    target_link_libraries(jarp PRIVATE SDL2main)
    add_custom_command(TARGET jarp POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:SDL2>
            $<TARGET_FILE_DIR:jarp>)
endif()

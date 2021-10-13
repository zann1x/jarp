cmake_minimum_required(VERSION 3.19)
project(jarp VERSION 0.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(lib)

add_executable(jarp "src/main.cpp" "src/Renderer.h" "src/Renderer.cpp" "src/Win32Window.cpp" "src/Win32Window.h" "src/FileSystem.h" "src/FileSystem.cpp"  "src/Shader.h" "src/Shader.cpp"  "src/Application.cpp" "src/ImGuiBuilder.cpp" "src/StbBuilder.cpp")
configure_file("src/VersionConfig.h.in" "VersionConfig.h")
target_link_libraries(jarp PRIVATE glad glm imgui SDL2 spdlog stb)
target_include_directories(jarp PUBLIC "${PROJECT_BINARY_DIR}")

if(WIN32)
    target_link_libraries(jarp PRIVATE SDL2main)
    add_custom_command(TARGET jarp POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:SDL2>
            $<TARGET_FILE_DIR:jarp>)
endif()

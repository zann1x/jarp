# See also https://onqtam.com/programming/2019-12-20-pch-unity-cmake-3-16/

cmake_minimum_required(VERSION 3.16)

project(jarp)
set(CMAKE_C_STANDARD 99)
set(CMAKE_UNITY_BUILD ON)

#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_compile_definitions(
    LEFT_HAND_COORDINATES
    ZERO_TO_ONE_COORDINATES
    JARP_PLATFORM_WIN32
    VK_USE_PLATFORM_WIN32_KHR
    _CRT_SECURE_NO_WARNINGS
    WIN32_LEAN_AND_MEAN
    NOMINMAX
)

# Include sub-projects.
add_subdirectory("jarp")
add_subdirectory("Game")
add_subdirectory("Sandbox")

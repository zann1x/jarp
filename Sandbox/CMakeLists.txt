cmake_minimum_required(VERSION 3.16)

include_directories(
    ${PROJECT_SOURCE_DIR}/Sandbox/Source
    ${PROJECT_SOURCE_DIR}/jarp/Source
)
add_library(Sandbox SHARED "Source/Sandbox/sandbox.c")

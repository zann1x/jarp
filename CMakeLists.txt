cmake_minimum_required(VERSION 3.16)

project(jarp)
set(CMAKE_C_STANDARD 99)
set(CMAKE_UNITY_BUILD ON)

set(BuildDir "${PROJECT_SOURCE_DIR}/Build")
set(BuildCacheDir "${PROJECT_SOURCE_DIR}/BuildCache")
set(BuildInstallDir "${PROJECT_SOURCE_DIR}/BuildInstall")

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_BINARY_DIR ${BuildCacheDir})
set(CMAKE_INSTALL_PREFIX ${BuildInstallDir})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BuildDir})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BuildDir})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${BuildDir})

set(J3rdPartyDir "${PROJECT_SOURCE_DIR}/jarp/ThirdParty")
set(JarpSourceDir "${PROJECT_SOURCE_DIR}/jarp/Source")
set(GameSourceDir "${PROJECT_SOURCE_DIR}/Game/Source")
set(SandboxSourceDir "${PROJECT_SOURCE_DIR}/Sandbox/Source")

set(SDL2Libs "${J3rdPartyDir}/SDL2/lib/x64/SDL2.lib;${J3rdPartyDir}/SDL2/lib/x64/SDL2main.lib")

set(Shaders
    ${CMAKE_SOURCE_DIR}/jarp/Shader/Basic.frag
    ${CMAKE_SOURCE_DIR}/jarp/Shader/Basic.vert
)

add_compile_definitions(
    LEFT_HAND_COORDINATES
    ZERO_TO_ONE_COORDINATES
    JARP_PLATFORM_WIN32
    VK_USE_PLATFORM_WIN32_KHR
    _CRT_SECURE_NO_WARNINGS
    WIN32_LEAN_AND_MEAN
    NOMINMAX
)

include_directories(
    ${PROJECT_SOURCE_DIR}/jarp/Source
    ${PROJECT_SOURCE_DIR}/Game/Source
    ${PROJECT_SOURCE_DIR}/Sandbox/Source
    ${J3rdPartyDir}/SDL2/include
    ${J3rdPartyDir}/stb
    ${J3rdPartyDir}/volk
    "C:/VulkanSDK/1.2.131/Include"
)

# jarp
add_executable(jarp "${JarpSourceDir}/jarp/platform/win32/win32_main.c")
add_custom_command(
    TARGET jarp
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${J3rdPartyDir}/SDL2/lib/x64/SDL2.dll" $<TARGET_FILE_DIR:jarp>
)
foreach(Shader ${Shaders})
    add_custom_command(
        TARGET jarp
        PRE_BUILD
        COMMAND glslangValidator ${Shader} -V -o ${Shader}.spv
        DEPENDS ${Shader}
    )
endforeach()
target_link_libraries(jarp ${SDL2Libs})

# Game
add_library(Game SHARED "${GameSourceDir}/game.c")
add_custom_command(
    TARGET Game
    PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E touch BuildLock.tmp
    COMMAND del Game-*.pdb
    WORKING_DIRECTORY "${BuildDir}"
    COMMENT "Creating build lock file & deleting pdb files"
)
add_custom_command(
    TARGET Game
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E remove BuildLock.tmp
    COMMAND ${CMAKE_COMMAND} -E rename Game.pdb Game-%random%.pdb
    WORKING_DIRECTORY "${BuildDir}"
    COMMENT "Deleting build lock file & renaming pdb file"
)

# Sandbox
add_library(Sandbox SHARED "${SandboxSourceDir}/Sandbox/sandbox.c")

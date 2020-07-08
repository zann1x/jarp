cmake_minimum_required(VERSION 3.16)

set(SDL2_LIBRARIES "${CMAKE_CURRENT_LIST_DIR}/ThirdParty/SDL2/lib/x64/SDL2.lib;${CMAKE_CURRENT_LIST_DIR}/ThirdParty/SDL2/lib/x64/SDL2main.lib")
set(SHADERS
    ${CMAKE_SOURCE_DIR}/jarp/Shader/Basic.frag
    ${CMAKE_SOURCE_DIR}/jarp/Shader/Basic.vert
)

include_directories(
    ${PROJECT_SOURCE_DIR}/jarp/Source
    ${PROJECT_SOURCE_DIR}/jarp/ThirdParty/SDL2/include
    ${PROJECT_SOURCE_DIR}/jarp/ThirdParty/stb
    ${PROJECT_SOURCE_DIR}/jarp/ThirdParty/volk
    "C:/VulkanSDK/1.2.131/Include"
)

add_executable(jarp "Source/jarp/platform/win32/win32_main.c")

add_custom_command(
    TARGET jarp
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_LIST_DIR}/ThirdParty/SDL2/lib/x64/SDL2.dll" $<TARGET_FILE_DIR:jarp>
)

foreach(SHADER ${SHADERS})
    add_custom_command(
        TARGET jarp
        PRE_BUILD
        COMMAND glslangValidator ${SHADER} -V -o ${SHADER}.spv
        DEPENDS ${SHADER}
    )
endforeach()

target_link_libraries(jarp ${SDL2_LIBRARIES})

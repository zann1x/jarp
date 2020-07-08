cmake_minimum_required(VERSION 3.16)

include_directories(
    ${PROJECT_SOURCE_DIR}/Game/Source
    ${PROJECT_SOURCE_DIR}/jarp/Source
)

add_library(Game SHARED "Source/game.c")

add_custom_command(
    TARGET Game
    PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E touch BuildLock.tmp
    COMMAND del Game-*.pdb
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/out/build/x64-Debug (default)/Game/"
    COMMENT "Creating build lock file & deleting pdb files"
)

add_custom_command(
    TARGET Game
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E remove BuildLock.tmp
    COMMAND ${CMAKE_COMMAND} -E rename Game.pdb Game-%random%.pdb
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/out/build/x64-Debug (default)/Game/"
    COMMENT "Deleting build lock file & renaming pdb file"
)

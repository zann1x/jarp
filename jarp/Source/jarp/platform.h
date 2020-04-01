#pragma once

#include "api_types.h"

#define PLATFORM_TEST(name) int name(void)
typedef PLATFORM_TEST(PlatformTest);

struct PlatformAPI {
    PlatformTest* test;
};

struct GameMemory {
    struct PlatformAPI platform_api;
};

#define GAME_UPDATE_AND_RENDER(name) void name(struct GameMemory* game_memory)
typedef GAME_UPDATE_AND_RENDER(GameUpdateAndRender);

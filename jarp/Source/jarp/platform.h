#ifndef PLATFORM_H
#define PLATFORM_H

#define PLATFORM_TEST(name) int name(void)
typedef PLATFORM_TEST(PlatformTest);

struct PlatformAPI {
    PlatformTest* test;
};

struct GameMemory {
    struct PlatformAPI platform_api;
};

#define GAME_UPDATE_AND_RENDER(name) void name()
typedef GAME_UPDATE_AND_RENDER(GameUpdateAndRender);

// General usage functions of the engine that can get called by the game.
struct GameImport {
    PlatformTest* test;
};

// Callback functions into the game.
// The functions here generally define the game loop.
struct GameExport {
    GameUpdateAndRender* update_and_render;
};

#define GAME_GET_API(name) struct GameExport* name(struct GameImport* game_import)
typedef GAME_GET_API(GameGetAPI);

#endif

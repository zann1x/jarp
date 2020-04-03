#include "jarp/api_types.h"
#include "jarp/platform.h"

#include <stdio.h>

struct GameExport globals;
struct GameImport gi;

API GAME_UPDATE_AND_RENDER(game_update_and_render) {
	printf("Updating the game... %d, calling the platform... %d\n", 42, gi.test());
}

API GAME_GET_API(game_get_api) {
	gi = *game_import;

	globals.update_and_render = game_update_and_render;

	return &globals;
}

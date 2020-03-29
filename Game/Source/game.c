#include "jarp/api_types.h"
#include "jarp/platform.h"

#include <stdio.h>

API GAME_UPDATE_AND_RENDER(game_update_and_render)
{
	printf("Updating the game... %d\n", 42);
}

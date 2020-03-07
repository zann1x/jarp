#include <stdio.h>

#include <SDL.h>

int main(int argc, char** argv)
{
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window* window = SDL_CreateWindow("Hello world", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);
	SDL_Surface* surface = SDL_GetWindowSurface(window);
	SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0xff, 0xdd, 0xaa));
	SDL_UpdateWindowSurface(window);
	SDL_Delay(5000);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}

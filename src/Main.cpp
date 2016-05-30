#include <SDL.h>

#include "Error.hpp"

void shutdownSDL(SDL_Window *window)
{
	if (window)
		SDL_DestroyWindow(window);

	SDL_Quit();
}

int main(int argc, char* args[])
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		Error::reportSDL("Failed to initialize SDL library.");
		shutdownSDL(nullptr);
		return -1;
	}
	
	SDL_Window *window = SDL_CreateWindow("Cognitive Crowd Simulation", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1024, 768, SDL_WINDOW_SHOWN);

	if (window == nullptr)
	{
		Error::reportSDL("Failed to create window.");
		shutdownSDL(nullptr);
		return -1;
	}
		
	SDL_Surface *screenSurface = SDL_GetWindowSurface(window);
	SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 255, 0, 255));
	SDL_UpdateWindowSurface(window);

	SDL_Delay(2000);
	
	shutdownSDL(window);

	return 0;
}
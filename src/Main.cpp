#include <cassert>
#include <SDL.h>

#include "Window.hpp"

int main(int argc, char *args[])
{
	Window *window = new Window("Cognitive Crowd Simulation", 1024, 768);
	
	if (!window)
		return 1;

	SDL_Renderer *renderer = window->getRenderer();
	SDL_Event event;
	while (true)
	{
		while (SDL_PollEvent(&event) != 0)
		{
			if (event.type == SDL_QUIT)
			{
				delete window;
				return 0;
			}
		}

		assert(renderer);
		SDL_SetRenderDrawColor(renderer, 64, 64, 196, 255);
		SDL_RenderClear(renderer);

		SDL_SetRenderDrawColor(renderer, 196, 32, 32, 255);
		SDL_Rect r = { 10, 10, 100, 100 };
		SDL_RenderDrawRect(renderer, &r);

		SDL_RenderPresent(renderer);
		SDL_Delay(25);
	}
	
	delete window;
	return 0;
}
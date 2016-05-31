#include <cassert>
#include <ctime>
#include <SDL.h>

#include "Scene.hpp"
#include "Window.hpp"

int main(int argc, char *args[])
{
	srand(unsigned int(time(nullptr)));

	Window *window = new Window("Cognitive Crowd Simulation", 1024, 768);
	
	if (!window)
		return 1;

	Scene scene;

	const float FIXED_FPS = 60.0f;
	const float DELTA_TIME = 1000.0f / FIXED_FPS;

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
			else if (event.type == SDL_KEYUP)
			{
				if (event.key.keysym.sym == SDLK_r)
					scene.scramblePedestrians(5);
			}
		}

		scene.update(DELTA_TIME);

		assert(renderer);
		SDL_SetRenderDrawColor(renderer, 96, 96, 224, 255);
		SDL_RenderClear(renderer);
		scene.render(renderer);
		SDL_RenderPresent(renderer);

		SDL_Delay(int(DELTA_TIME));
	}
	
	delete window;
	return 0;
}
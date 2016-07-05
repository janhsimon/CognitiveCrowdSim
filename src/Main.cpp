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

	glm::vec2 mouseDownLocation;
	bool debug = true;

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
			else if (event.type == SDL_MOUSEBUTTONDOWN)
			{
				mouseDownLocation = glm::vec2(event.motion.x, event.motion.y);
			}
			else if (event.type == SDL_MOUSEBUTTONUP)
			{
				glm::vec2 mouseUpLocation(event.motion.x, event.motion.y);

				if (event.button.button == 1)
					scene.spawnPedestrian(mouseDownLocation, mouseUpLocation);
				else if (event.button.button == 3)
					scene.spawnWall(mouseDownLocation, mouseUpLocation);
			}
			else if (event.type == SDL_KEYUP)
			{
				if (event.key.keysym.sym == SDLK_r)
					scene.resetPedestrians();
				else if (event.key.keysym.sym == SDLK_SPACE)
					scene.scramblePedestrians(10);
				else if (event.key.keysym.sym == SDLK_c)
					scene.spawnCircleOfPedestrians();
				else if (event.key.keysym.sym == SDLK_w)
					scene.spawnWallOfPedestrians();
				else if (event.key.keysym.sym == SDLK_F1)
					debug = !debug;
			}
		}

		scene.update(DELTA_TIME);

		assert(renderer);
		SDL_SetRenderDrawColor(renderer, 96, 96, 224, 255);
		SDL_RenderClear(renderer);
		scene.render(renderer, debug);
		SDL_RenderPresent(renderer);

		SDL_Delay(int(DELTA_TIME));
	}
	
	delete window;
	return 0;
}
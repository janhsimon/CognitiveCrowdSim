#include "Error.hpp"
#include "Window.hpp"

Window::Window(std::string title, unsigned int width, unsigned int height)
{
	if (window)
		SDL_DestroyWindow(window);

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		Error::reportSDL("Failed to initialize SDL library.");
		SDL_Quit();
		return;
	}

	window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);

	if (!window)
	{
		Error::reportSDL("Failed to create window.");
		SDL_Quit();
		return;
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	if (!renderer)
	{
		Error::reportSDL("Failed to create renderer.");
		SDL_DestroyWindow(window);
		SDL_Quit();
		return;
	}
}

Window::~Window()
{
	if (renderer)
		SDL_DestroyRenderer(renderer);

	if (window)
		SDL_DestroyWindow(window);

	SDL_Quit();
}
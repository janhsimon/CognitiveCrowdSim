#pragma once

#include <SDL.h>
#include <string>

class Window
{
private:
	SDL_Window *window;
	SDL_Renderer *renderer;

public:
	Window(std::string title, unsigned int width, unsigned int height);
	~Window();

	inline SDL_Renderer *getRenderer() const { return renderer; }
};
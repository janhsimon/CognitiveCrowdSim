#include <SDL.h>
#include <sstream>

#include "Error.hpp"

void Error::report(std::string text)
{
	SDL_ShowSimpleMessageBox(0, "Error", text.c_str(), nullptr);
}

void Error::reportSDL(std::string text)
{
	std::string sdlError = SDL_GetError();

	if (!sdlError.empty())
		report(text + " SDL error: " + sdlError);
	else
		report(text);
}
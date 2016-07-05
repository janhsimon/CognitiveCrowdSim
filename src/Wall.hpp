#pragma once

#include <glm.hpp>
#include <SDL.h>

class Wall
{
private:
	glm::vec2 from, to;

public:
	Wall(const glm::vec2 &from, const glm::vec2 &to);

	inline glm::vec2 getFrom() const { return from; }
	inline glm::vec2 getTo() const { return to; }
	
	void render(SDL_Renderer *renderer);
};
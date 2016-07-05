#include "Wall.hpp"

Wall::Wall(const glm::vec2 &from, const glm::vec2 &to)
{
	this->from = from;
	this->to = to;
}

void Wall::render(SDL_Renderer *renderer)
{
	assert(renderer);

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderDrawLine(renderer, int(from.x), int(from.y), int(to.x), int(to.y));
}
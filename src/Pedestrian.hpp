#pragma once

#include <glm.hpp>
#include <SDL.h>

class Pedestrian
{
private:
	glm::vec2 position, velocity;
	float mass;
	float radius;
	float bestWalkingSpeed;
	glm::vec2 destinationPoint;

public:
	Pedestrian();

	void update(float deltaTime);
	void render(SDL_Renderer *renderer);
};
#include <cassert>
#include <cmath>
#include <gtc\random.hpp>

#include "Pedestrian.hpp"

void drawCircle(SDL_Renderer *renderer, float x, float y, float radius, int numPoints)
{
	assert(renderer);
	assert(numPoints > 2);

	const float CIRCLE_INCREMENT = glm::radians(360.0f / numPoints);

	SDL_Point *points = new SDL_Point[numPoints + 1];

	for (int i = 0; i < numPoints; ++i)
	{
		const float ANGLE_IN_RADIANS = i * CIRCLE_INCREMENT;
		points[i].x = int(x + sinf(ANGLE_IN_RADIANS) * radius);
		points[i].y = int(y + cosf(ANGLE_IN_RADIANS) * radius);
	}

	points[numPoints] = points[0];

	SDL_RenderDrawLines(renderer, points, numPoints + 1);

	delete[] points;
}

Pedestrian::Pedestrian()
{
	position = glm::linearRand(glm::vec2(0.0f, 0.0f), glm::vec2(1024.0f, 768.0f));
	velocity = glm::vec2(0.0f, 0.0f);
	mass = glm::linearRand(40.0f, 140.0f);
	radius = mass / 8.0f;
	bestWalkingSpeed = glm::linearRand(0.04f, 0.1f);
	destinationPoint = glm::linearRand(glm::vec2(0.0f, 0.0f), glm::vec2(1024.0f, 768.0f));
}

void Pedestrian::update(float deltaTime)
{
	glm::vec2 pedToDest = destinationPoint - position;

	if (glm::length(pedToDest) < bestWalkingSpeed * deltaTime)
	{
		position = destinationPoint;
		return;
	}
	
	velocity = glm::normalize(pedToDest) * bestWalkingSpeed;

	position += velocity * deltaTime;
}

void Pedestrian::render(SDL_Renderer *renderer)
{
	assert(renderer);

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	drawCircle(renderer, position.x, position.y, radius, 16);

	SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
	SDL_RenderDrawLine(renderer, int(position.x), int(position.y), int(destinationPoint.x), int(destinationPoint.y));
}
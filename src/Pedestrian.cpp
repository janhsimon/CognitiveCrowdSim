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

Pedestrian::Pedestrian() : Pedestrian(glm::vec2(0.0f, 0.0f))
{
	position = glm::linearRand(glm::vec2(0.0f, 0.0f), glm::vec2(1024.0f, 768.0f));
	destinationPoint = glm::linearRand(glm::vec2(0.0f, 0.0f), glm::vec2(1024.0f, 768.0f));
}

Pedestrian::Pedestrian(const glm::vec2 &position) : Pedestrian(position, glm::vec2(0.0f, 0.0f))
{
	destinationPoint = glm::linearRand(glm::vec2(0.0f, 0.0f), glm::vec2(1024.0f, 768.0f));
}

Pedestrian::Pedestrian(const glm::vec2 &position, const glm::vec2 &destinationPoint)
{
	this->position = position;
	velocity = glm::vec2(0.0f, 0.0f);
	mass = glm::linearRand(40.0f, 140.0f);
	radius = mass / 8.0f;
	forward = glm::normalize(destinationPoint - position);
	bestWalkingSpeed = glm::linearRand(0.1f, 0.2f);
	walkingSpeed = bestWalkingSpeed;
	this->destinationPoint = destinationPoint;
	fieldOfView = 400.0f;
	visionDepth = 600.0f;

	///numRays = 48;//128;
	//rays = new glm::vec2[numRays];
}

/*
void Pedestrian::calculateRays()
{
	assert(rays);

	glm::vec2 pedToDest = destinationPoint - position;
	pedToDest = glm::normalize(pedToDest);

	for (unsigned int i = 0; i < numRays; ++i)
	{
		float shiftAmount = (float(i) / numRays) * (fieldOfView * 2.0f) - fieldOfView;
		rays[i] = pedToDest * visionDepth + glm::vec2(pedToDest.y, -pedToDest.x) * shiftAmount;
	}
}
*/

void Pedestrian::update(float deltaTime)
{
	glm::vec2 pedToDest = destinationPoint - position;

	if (glm::length(pedToDest) < bestWalkingSpeed * deltaTime)
		forward = glm::vec2(0.0f, 0.0f);

	position += forward * walkingSpeed * deltaTime;
}

void Pedestrian::render(SDL_Renderer *renderer)
{
	assert(renderer);

	if (walkingSpeed >= bestWalkingSpeed)
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	else
		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

	drawCircle(renderer, position.x, position.y, radius, 16);

	/*
	assert(rays);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	for (unsigned int i = 0; i < numRays; ++i)
		SDL_RenderDrawLine(renderer, int(position.x), int(position.y), int(position.x + rays[i].x), int(position.y + rays[i].y));
	*/

	SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
	drawCircle(renderer, destinationPoint.x, destinationPoint.y, 5.0f, 4);

	SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
	SDL_RenderDrawLine(renderer, int(position.x), int(position.y), int(position.x + forward.x * 100.0f), int(position.y + forward.y * 100.0f));
}
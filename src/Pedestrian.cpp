#include <cassert>
#include <cmath>
#include <gtc\random.hpp>
#include <iostream>

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
	acceleration = destinationPoint - position; //glm::vec2(0.0f, 0.0f);
	mass = glm::linearRand(40.0f, 140.0f);
	radius = mass / 8.0f;
	bestWalkingSpeed = glm::linearRand(1.0f, 2.0f);
	this->destinationPoint = destinationPoint;
	//fieldOfView = 400.0f;
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

void Pedestrian::update(const glm::vec2 &v_des, float deltaTime)
{
	/*
	glm::vec2 pedToDest = destinationPoint - position;

	if (glm::length(pedToDest) < bestWalkingSpeed * deltaTime)
		forward = glm::vec2(0.0f, 0.0f);
	*/

	/*
	velocity += acceleration * deltaTime;

	if (glm::length(velocity) > bestWalkingSpeed)
		velocity = glm::normalize(velocity) * bestWalkingSpeed;

	position += velocity * deltaTime;
	*/

	const float TAU = 2.0f;

	acceleration = (v_des - velocity) / TAU;

	//if (glm::length(acceleration) < 0.0001f)
		//acceleration = glm::vec2(0.0f, 0.0f);

	//std::cout << "v_des = " << v_des.x << "/" << v_des.y << "\tvelocity = " << velocity.x << "/" << velocity.y << "\taccel = " << acceleration.x << "/" << acceleration.y << std::endl;
	
	velocity += acceleration;

	if (glm::length(velocity) > bestWalkingSpeed)
		velocity = glm::normalize(velocity) * bestWalkingSpeed;

	position += velocity;
}

void Pedestrian::render(SDL_Renderer *renderer, bool debug)
{
	assert(renderer);

	if (glm::length(velocity) >= bestWalkingSpeed - FLT_EPSILON)
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	else
		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

	drawCircle(renderer, position.x, position.y, radius, 12);

	if (!debug)
		return;

	/*
	assert(rays);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	for (unsigned int i = 0; i < numRays; ++i)
		SDL_RenderDrawLine(renderer, int(position.x), int(position.y), int(position.x + rays[i].x), int(position.y + rays[i].y));
	*/

	SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
	drawCircle(renderer, destinationPoint.x, destinationPoint.y, 5.0f, 4);
	SDL_RenderDrawLine(renderer, int(position.x), int(position.y), int(destinationPoint.x), int(destinationPoint.y));

	float vX = glm::normalize(velocity).x;
	float vY = glm::normalize(velocity).y;

	glm::vec2 normAccel = glm::normalize(acceleration);

	float aX = 0.0f;
	float aY = 0.0f;

	if (glm::length(acceleration) > 0.1f)
	{
		aX = normAccel.x;
		aY = normAccel.y;
	}

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderDrawLine(renderer, int(position.x), int(position.y), int(position.x + vX * 100.0f), int(position.y + vY * 100.0f));

	SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
	SDL_RenderDrawLine(renderer, int(position.x), int(position.y), int(position.x + aX * 100.0f), int(position.y + aY * 100.0f));
}
#include <iostream>
#include <glm.hpp>

#include "Scene.hpp"

bool lineCircleIntersection(const glm::vec2 &lineStart, const glm::vec2 &lineEnd, const glm::vec2 &circleCenter, float circleRadius)
{
	glm::vec2 d = lineEnd - lineStart;
	glm::vec2 f = lineStart - circleCenter;

	float a = glm::dot(d, d);
	float b = 2.0f * glm::dot(f, d);
	float c = glm::dot(f, f) - circleRadius * circleRadius;

	float discriminant = b * b - 4.0f * a * c;

	if (discriminant >= 0.0f)
	{
		discriminant = sqrt(discriminant);

		float t1 = (-b - discriminant) / (2 * a);
		float t2 = (-b + discriminant) / (2 * a);

		if ((t1 >= 0 && t1 <= 1) || (t2 >= 0 && t2 <= 1))
			return true;
	}

	return false;
}

Scene::~Scene()
{
	pedestrians.clear();
}

void Scene::spawnPedestrian(const glm::vec2 &position, const glm::vec2 &destinationPoint)
{
	pedestrians.push_back(new Pedestrian(position, destinationPoint));
}

void Scene::scramblePedestrians(unsigned int count)
{
	while (count > 0)
	{
		pedestrians.push_back(new Pedestrian());
		count--;
	}
}

void Scene::resetPedestrians()
{
	pedestrians.clear();
}

void Scene::update(float deltaTime)
{
	for (Pedestrian *pedestrian : pedestrians)
	{
		glm::vec2 pedToDest = pedestrian->getDestinationPoint() - pedestrian->getPosition();
		pedestrian->calculateRays();

		float min_d_a = -1.0f;
		unsigned int preferredRayIndex;

		for (unsigned int i = 0; i < pedestrian->getNumRays(); ++i)
		{
			glm::vec2 ray = pedestrian->getRay(i);

			for (Pedestrian *obstacle : pedestrians)
			{
				if (pedestrian == obstacle)
					continue;

				if (lineCircleIntersection(pedestrian->getPosition(), pedestrian->getPosition() + ray, obstacle->getPosition(), obstacle->getRadius()))
				{
					float d_max = pedestrian->getVisionDepth();
					float f_a = glm::length(obstacle->getPosition() - pedestrian->getPosition());
					float a0 = atan2f(pedToDest.y, pedToDest.x);
					float a = atan2f(ray.y, ray.x);
					float d_a = d_max * d_max + f_a * f_a - 2.0f * d_max * f_a * cosf(a0 - a);

					if (min_d_a < 0.0f || d_a < min_d_a)
					{
						min_d_a = d_a;
						preferredRayIndex = i;
					}
				}
			}
		}

		if (min_d_a >= 0.0f)
		{
			glm::vec2 forward = pedestrian->getRay(preferredRayIndex);
			pedestrian->setForward(glm::normalize(forward));
		}
		else
			pedestrian->setForward(glm::normalize(pedToDest));

		pedestrian->update(deltaTime);
	}
}

void Scene::render(SDL_Renderer *renderer)
{
	for (Pedestrian *pedestrian : pedestrians)
		pedestrian->render(renderer);
}
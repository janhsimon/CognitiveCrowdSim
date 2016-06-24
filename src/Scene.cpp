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

float f(float a, const Pedestrian &pedestrian, const Scene *scene, float deltaTime)
{
	glm::vec2 v = glm::vec2(cosf(a), sinf(a));

	for (Pedestrian *obstacle : scene->pedestrians)
	{
		if (&pedestrian == obstacle)
			continue;

		// includes simulation of ped to move towards the a angle direction one time step and same for the obstacles with their current 
		if (lineCircleIntersection(pedestrian.getPosition() + v * pedestrian.getBestWalkingSpeed() * deltaTime, pedestrian.getPosition() + v * pedestrian.getVisionDepth(),
			obstacle->getPosition() + obstacle->getForward() * obstacle->getBestWalkingSpeed() * deltaTime, obstacle->getRadius() * 2.0f))
		{
			glm::vec2 pedToObstacle = obstacle->getPosition() - pedestrian.getPosition();
			//std::cout << "f(" << a << ") = " << glm::length(pedToObstacle) << "   and hit something" << std::endl;
			return glm::length(pedToObstacle);
		}
	}

	//std::cout << "f(" << a << ") = " << pedestrian.getVisionDepth() << "   and hit nothing" << std::endl;

	return pedestrian.getVisionDepth();
}

float d(float a, float a0, const Pedestrian &pedestrian, const Scene *scene, float deltaTime)
{
	assert(scene);
	float d_max = pedestrian.getVisionDepth();
	float f_a = f(a, pedestrian, scene, deltaTime);
	float d_a = d_max * d_max + f_a * f_a - 2.0f * d_max * f_a * cosf(a0 - a);
	//std::cout << "d(" << a << ") = " << d_a << "   with a0 = " << a0 << ", a0 - a = " << a0 - a << " and cosf(a0 - a) = " << cosf(a0 - a) << std::endl;
	return d_a;
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

void Scene::spawnCircleOfPedestrians()
{
	const int NUM_PEDESTRIANS = 12;
	const glm::vec2 CENTER(1024 / 2.0f, 768 / 2.0f);
	const float RADIUS = 250.0f;

	const float CIRCLE_INCREMENT = glm::radians(360.0f / NUM_PEDESTRIANS);

	for (int i = 0; i < NUM_PEDESTRIANS; ++i)
	{
		const float ANGLE_IN_RADIANS = i * CIRCLE_INCREMENT;

		glm::vec2 spawnPosition(CENTER.x + sinf(ANGLE_IN_RADIANS) * RADIUS, CENTER.y + cosf(ANGLE_IN_RADIANS) * RADIUS);
		glm::vec2 destinationPoint(CENTER.x + sinf(ANGLE_IN_RADIANS + M_PI) * RADIUS, CENTER.y + cosf(ANGLE_IN_RADIANS + M_PI) * RADIUS);
		spawnPedestrian(spawnPosition, destinationPoint);
	}
}

void Scene::spawnWallOfPedestrians()
{
	const int NUM_PEDESTRIANS = 8;
	const glm::vec2 CENTER(1024 / 2.0f, 768 / 2.0f);
	const float DISTANCE = 250.0f;
	const float HALF_DISTANCE = DISTANCE / 2.0f;

	for (int i = 0; i < NUM_PEDESTRIANS; ++i)
	{
		glm::vec2 leftPosition(CENTER.x - DISTANCE, CENTER.y - HALF_DISTANCE + ((i / (float)NUM_PEDESTRIANS) * DISTANCE));
		glm::vec2 rightPosition(CENTER.x + DISTANCE, CENTER.y - HALF_DISTANCE + ((i / (float)NUM_PEDESTRIANS) * DISTANCE));
		spawnPedestrian(leftPosition, rightPosition);
		spawnPedestrian(rightPosition, leftPosition);
	}
}

void Scene::resetPedestrians()
{
	pedestrians.clear();
}

void Scene::update(float deltaTime)
{
	const float THETA = M_PI / 2.0f;
	const float STEP_SIZE = THETA / 256.0f;

	for (Pedestrian *pedestrian : pedestrians)
	{
		glm::vec2 pedToDest = glm::normalize(pedestrian->getDestinationPoint() - pedestrian->getPosition());
		glm::vec2 forward = glm::normalize(pedestrian->getForward());
		
		float a0 = atan2f(pedToDest.y, pedToDest.x);
		float H = atan2f(forward.y, forward.x);

		//std::cout << "H = " << H << std::endl;

		float min_d_a = -1.0f;
		float min_a = -1.0f;
		for (float a = H - THETA; a < H + THETA; a += STEP_SIZE)
		{
			float d_a = d(a, a0, *pedestrian, this, deltaTime);

			if (d_a < min_d_a || min_d_a < 0.0f)
			{
				min_d_a = d_a;
				min_a = a;
			}
		}
		
		//std::cout << "selected angle = " << min_a << std::endl;
		glm::vec2 newForward(cosf(min_a), sinf(min_a));
		pedestrian->setForward(glm::normalize(newForward));

		pedestrian->update(deltaTime);
	}

	for (Pedestrian *pedestrian : pedestrians)
	{
		for (Pedestrian *collider : pedestrians)
		{
			if (pedestrian == collider)
				continue;
			
			glm::vec2 pedToColl = collider->getPosition() - pedestrian->getPosition();

			const float MIN_DISTANCE = pedestrian->getRadius() + collider->getRadius();
			float distance = glm::length(pedToColl);
			pedToColl = glm::normalize(pedToColl);

			if (distance < MIN_DISTANCE)
			{
				float penetration = MIN_DISTANCE - distance;
				pedestrian->setPosition(pedestrian->getPosition() - pedToColl * penetration * 0.5f);
				collider->setPosition(collider->getPosition() + pedToColl * penetration * 0.5f);
			}
		}
	}
}

void Scene::render(SDL_Renderer *renderer)
{
	for (Pedestrian *pedestrian : pedestrians)
		pedestrian->render(renderer);
}
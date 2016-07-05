#include <iostream>
#include <glm.hpp>

#include "Scene.hpp"

bool lineCircleIntersection(const glm::vec2 &lineStart, const glm::vec2 &lineEnd, const glm::vec2 &circleCenter, float circleRadius, glm::vec2 &outHit)
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

		if (t1 >= 0 && t1 <= 1)
		{
			outHit = d * t1;
			return true;
		}
		else if (t2 >= 0 && t2 <= 1)
		{
			outHit = d * t2;
			return true;
		}
	}

	return false;
}

bool lineLineIntersection(const glm::vec2 &line1Start, const glm::vec2 &line1End, const glm::vec2 &line2Start, const glm::vec2 &line2End, glm::vec2 &outHit)
{
	float p0_x = line1Start.x;	float p0_y = line1Start.y;
	float p1_x = line1End.x;	float p1_y = line1End.y;
	float p2_x = line2Start.x;	float p2_y = line2Start.y;
	float p3_x = line2End.x;	float p3_y = line2End.y;

	float s1_x, s1_y, s2_x, s2_y;
	s1_x = p1_x - p0_x;	s1_y = p1_y - p0_y;
	s2_x = p3_x - p2_x;	s2_y = p3_y - p2_y;

	float s, t;
	s = (-s1_y * (p0_x - p2_x) + s1_x * (p0_y - p2_y)) / (-s2_x * s1_y + s1_x * s2_y);
	t = (s2_x * (p0_y - p2_y) - s2_y * (p0_x - p2_x)) / (-s2_x * s1_y + s1_x * s2_y);

	if (s >= 0 && s <= 1 && t >= 0 && t <= 1)
	{
		outHit = glm::vec2(p0_x + (t * s1_x), p0_y + (t * s1_y));
		return true;
	}

	return false;
}

float f(float a, const Pedestrian &pedestrian, const Scene *scene, float deltaTime)
{
	float timeToCollision = 1.0f;
	const float SAFETY_MARGIN = 1.1f;

	glm::vec2 v = glm::vec2(cosf(a), sinf(a));
	v = glm::normalize(v);

	float out = pedestrian.getVisionDepth();

	glm::vec2 simulatedStart = pedestrian.getPosition() + v * pedestrian.getBestWalkingSpeed() * timeToCollision;

	for (Pedestrian *otherPedestrian : scene->pedestrians)
	{
		if (&pedestrian == otherPedestrian)
			continue;

		glm::vec2 hit;

		if (lineCircleIntersection(simulatedStart, simulatedStart + v * pedestrian.getVisionDepth(),
			otherPedestrian->getPosition() + otherPedestrian->getVelocity() * timeToCollision, (otherPedestrian->getRadius() + pedestrian.getRadius()) * SAFETY_MARGIN, hit))
		{
			glm::vec2 pedToHit = /*hit*/otherPedestrian->getPosition() - simulatedStart;
			//std::cout << "f(" << a << ") = " << glm::length(pedToHit) << "   and hit a pedestrian" << std::endl;

			float h = glm::length(pedToHit) - otherPedestrian->getRadius();

			if (h < out)
				out = h;
		}
	}

	for (Wall *wall : scene->walls)
	{
		glm::vec2 hit;

		glm::vec2 wallVector = wall->getTo() - wall->getFrom();
		wallVector = glm::normalize(wallVector) * pedestrian.getRadius();

		if (lineLineIntersection(simulatedStart, simulatedStart + v * pedestrian.getVisionDepth(),
			wall->getFrom() - wallVector, wall->getTo() + wallVector, hit))
		{
			glm::vec2 pedToHit = hit - simulatedStart;
			//std::cout << "f(" << a << ") = " << glm::length(pedToHit) << "   and hit a wall" << std::endl;

			float h = glm::length(pedToHit);

			if (h < out)
				out = h;
		}
	}

	//std::cout << "f(" << a << ") = " << out << "   and hit nothing" << std::endl;

	return out;
}

float d(float a, float a0, const Pedestrian &pedestrian, const Scene *scene, float deltaTime, float &outF)
{
	assert(scene);
	float d_max = pedestrian.getVisionDepth();
	float f_a = f(a, pedestrian, scene, deltaTime);
	float d_a = d_max * d_max + f_a * f_a - 2.0f * d_max * f_a * cosf(a0 - a);
	//std::cout << "d(" << a << ") = " << d_a << "   with a0 = " << a0 << ", a0 - a = " << a0 - a << " and cosf(a0 - a) = " << cosf(a0 - a) << std::endl;
	outF = f_a;
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
	const int NUM_PEDESTRIANS = 10;
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
	const int NUM_PEDESTRIANS = 6;
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
	walls.clear();
}

void Scene::spawnWall(const glm::vec2 &from, const glm::vec2 &to)
{
	walls.push_back(new Wall(from, to));
}

void Scene::update(float deltaTime)
{
	const float THETA = M_PI / 2.0f;
	const float STEP_SIZE = THETA / 64.0f;

	const float TAU = 2.0f;

	for (Pedestrian *pedestrian : pedestrians)
	{
		glm::vec2 pedToDest = glm::normalize(pedestrian->getDestinationPoint() - pedestrian->getPosition());
		glm::vec2 forward = glm::normalize(pedestrian->getVelocity());
		
		float a0 = atan2f(pedToDest.y, pedToDest.x);
		float H = atan2f(forward.y, forward.x);

		//std::cout << "H = " << H << std::endl;

		float min_d_a = -1.0f;
		float min_a = -1.0f;
		float min_f = -1.0f;
		for (float a = H - THETA; a < H + THETA; a += STEP_SIZE)
		{
			float f;
			float d_a = d(a, a0, *pedestrian, this, deltaTime, f);

			if (d_a < min_d_a || min_d_a < 0.0f)
			{
				min_d_a = d_a;
				min_a = a;
				min_f = f;
			}
		}
		
		// calculate a_des angle
		float a_des = min_a;
		// std::cout << "a_des (in degrees) = " << glm::degrees(min_a) << std::endl;

		// calculate v_des speed
		float d_h = min_f;
		//std::cout << "d_h = " << d_h << std::endl;
		float v_des_speed = glm::min(pedestrian->getBestWalkingSpeed(), d_h / TAU);

		// calculate v_des vector
		glm::vec2 v_des(cosf(a_des), sinf(a_des));
		//std::cout << "v_des = " << v_des.x << "/" << v_des.y << std::endl;
		v_des = glm::normalize(v_des) * v_des_speed;
		//std::cout << "v_des = " << v_des.x << "/" << v_des.y << std::endl;

		//glm::vec2 pedToObstacle = pedestrian->getVelocity() * min_f;
		//float walkingSpeed = glm::min(pedestrian->getBestWalkingSpeed(), (glm::length(pedToObstacle) / pedestrian->getVisionDepth()) * TAU);

		//pedestrian->setAcceleration(glm::normalize(newForward) * walkingSpeed);

		pedestrian->update(v_des, deltaTime);
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

void Scene::render(SDL_Renderer *renderer, bool debug)
{
	for (Wall *wall : walls)
		wall->render(renderer);

	for (Pedestrian *pedestrian : pedestrians)
		pedestrian->render(renderer, debug);
}
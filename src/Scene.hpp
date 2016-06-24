#pragma once

#include <vector>

#include "Pedestrian.hpp"

class Scene
{
public: // TODO: hack for now
	std::vector<Pedestrian*> pedestrians;
		
public:
	~Scene();

	void spawnPedestrian(const glm::vec2 &position, const glm::vec2 &destinationPoint);
	void scramblePedestrians(unsigned int count);
	void spawnCircleOfPedestrians();
	void spawnWallOfPedestrians();
	void resetPedestrians();
	void update(float deltaTime);
	void render(SDL_Renderer *renderer);
};
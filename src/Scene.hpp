#pragma once

#include <vector>

#include "Pedestrian.hpp"
#include "Wall.hpp"

class Scene
{
public: // TODO: hack for now
	std::vector<Pedestrian*> pedestrians;
	std::vector<Wall*> walls;
		
public:
	~Scene();

	void spawnPedestrian(const glm::vec2 &position, const glm::vec2 &destinationPoint);
	void scramblePedestrians(unsigned int count);
	void spawnCircleOfPedestrians();
	void spawnWallOfPedestrians();
	void resetPedestrians();
	void spawnWall(const glm::vec2 &from, const glm::vec2 &to);
	void update(float deltaTime);
	void render(SDL_Renderer *renderer, bool debug);
};
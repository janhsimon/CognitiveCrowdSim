#pragma once

#include <vector>

#include "Pedestrian.hpp"

class Scene
{
private:
	std::vector<Pedestrian*> pedestrians;
		
public:
	~Scene();

	void scramblePedestrians(unsigned int count);
	void update(float deltaTime);
	void render(SDL_Renderer *renderer);
};
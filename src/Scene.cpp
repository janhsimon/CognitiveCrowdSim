#include "Scene.hpp"

Scene::~Scene()
{
	pedestrians.clear();
}

void Scene::scramblePedestrians(unsigned int count)
{
	pedestrians.clear();

	while (count > 0)
	{
		pedestrians.push_back(new Pedestrian());
		count--;
	}
}

void Scene::update(float deltaTime)
{
	for (Pedestrian *pedestrian : pedestrians)
		pedestrian->update(deltaTime);
}

void Scene::render(SDL_Renderer *renderer)
{
	for (Pedestrian *pedestrian : pedestrians)
		pedestrian->render(renderer);
}
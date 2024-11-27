#pragma once
#include "Light.h"
#include "Shape.h"

#include <optional>
#include <vector>


class Scene
{
public:
	Scene(std::vector<Shape*>&& _shapes, std::vector<Light*>&& _lights);
	~Scene();
	float Hit(Ray r, float t0, float t1, MaterialRecord& rec);
	std::vector<Light*> getLights();
private:
	std::vector<Shape*> shapes;
	std::vector<Light*> lights;

};

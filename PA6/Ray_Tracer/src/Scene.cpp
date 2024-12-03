#include "Scene.h"

Scene::~Scene()
{
	for (Shape* s : shapes) {
		delete s;
	}
	for (Light* s : lights) {
		delete s;
	}
}

float Scene::Hit(Ray r, float t0, float t1, MaterialRecord& rec)
{
	float finalT = INFINITY;
	Shape* curr = nullptr;
	for (Shape* s : shapes) {
		float t = s->Intersect(r, t0, t1);
		if (t < finalT) {
			curr = s;
			finalT = t;
		}
	}
	if (finalT > t1 || finalT < t0)
		return INFINITY;
	rec = curr->GetParameters(r.getPosition(finalT));
	return finalT;
}

std::vector<Light*> Scene::getLights()
{
	return lights;
}

Scene::Scene(std::vector<Shape*>&& _shapes, std::vector<Light*>&& _lights) :
	shapes(std::move(_shapes)),
	lights(std::move(_lights))
{}

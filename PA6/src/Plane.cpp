#include "Plane.h"



Plane::Plane(const glm::vec3& _center,
	const glm::vec3& _normal,
	const glm::vec3& _ka,
	const glm::vec3& _kd,
	const glm::vec3& _ks,
	const glm::vec3& _km,
	float _n)
	: Shape(_ka, _kd, _ks, _km, _n),
	center(_center),
	normal(_normal)
{}

Plane::~Plane()
{
}

float Plane::Intersect(Ray ray, float t0, float t1)
{
	float t = glm::dot(center - ray.o, normal);
	t /= glm::dot(ray.d, normal);
	if (t >= t0 && t <= t1) {
		return t;
	}
	return INFINITY;
}

glm::vec3 Plane::getNormal(glm::vec3 position)
{
	return normal;
}

MaterialRecord Plane::GetParameters(glm::vec3 position)
{
	return MaterialRecord(ka, kd, ks, km, n, getNormal(position));
}


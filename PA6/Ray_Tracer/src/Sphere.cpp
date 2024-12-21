#include "Sphere.h"
#include <iostream>
Sphere::Sphere(const glm::vec3& _center,
	float _radius,
	const glm::vec3& _ka,
	const glm::vec3& _kd,
	const glm::vec3& _ks,
	const glm::vec3& _km,
	const glm::vec3& _kt,
	float _n, float _n2) :
	Center(_center),
	Radius(_radius),
	Shape(_ka, _kd, _ks, _km, _n, _kt, _n2)
{
}

Sphere::~Sphere()
{
}

float Sphere::Intersect(Ray ray, float t0, float t1)
{
	float a = glm::dot(ray.d, ray.d);
	float b = 2 * glm::dot((ray.o - Center), ray.d);
	float c = glm::dot(ray.o - Center, ray.o - Center) - Radius * Radius;
	float val1 = b * b - 4 * a * c;
	if (val1 < 0) {
		//std::cout << "Bad Val\n";
		return INFINITY;
	}
	val1 = sqrt(val1);
	float T1 = (-1 * b + val1) * 1.0f / (2 * a);
	float T2 = (-1 * b - val1) * 1.0f / (2 * a);
	if (T1 > T2) {
		std::swap(T1, T2);
	}
	float finalT = std::min(T1, T2);
	if (finalT < t0 && T2 < t0) {
		return INFINITY;
	}
	if (finalT > t1) {
		return INFINITY;
	}
	return finalT;
}

glm::vec3 Sphere::getNormal(glm::vec3 position)
{
	return glm::normalize(position - Center);
}

MaterialRecord Sphere::GetParameters(glm::vec3 position)
{
	return MaterialRecord(ka, kd, ks, km, n, kt, ior, getNormal(position));
}


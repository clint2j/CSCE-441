#pragma once
#include "Shape.h"
class Plane :
	public Shape
{
public:
	Plane(const glm::vec3& _center,
		const glm::vec3& _normal,
		const glm::vec3& _ka,
		const glm::vec3& _kd,
		const glm::vec3& _ks,
		const glm::vec3& _km,
		float _n);
	~Plane();
	float Intersect(Ray ray, float t0, float t1) override;
	glm::vec3 getNormal(glm::vec3 position) override;
	MaterialRecord GetParameters(glm::vec3 position) override;
private:
	glm::vec3 center, normal;
	//glm::vec3 ka, kd, ks, km;
	//float n;
};
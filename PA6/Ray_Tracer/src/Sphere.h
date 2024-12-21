#pragma once
#include "Shape.h"
class Sphere :
	public Shape
{
public:
	Sphere(const glm::vec3& _center,
		float _radius,
		const glm::vec3& _ka,
		const glm::vec3& _kd,
		const glm::vec3& _ks,
		const glm::vec3& _km,
		const glm::vec3& _kt,
		float _n, float _n2);
	~Sphere();
	float Intersect(Ray ray, float t0, float t1) override;
	glm::vec3 getNormal(glm::vec3 position) override;
	MaterialRecord GetParameters(glm::vec3 position) override;
private:
	glm::vec3 Center;
	float Radius;
	//glm::vec3 ka, kd, ks, km;
	//float n;
};
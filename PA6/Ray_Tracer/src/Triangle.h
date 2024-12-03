#pragma once

#include <array>
#include <math.h>
#include <optional>
#include <stdlib.h>
#include <vector>

#include <glm/glm.hpp>

#include "Shape.h"

struct Vertex {
	glm::vec3 pos;
	glm::vec3 norm;
};

class Triangle : public Shape
{
private:
	std::array<Vertex, 3> vertices;

public:
	// Constructor
	Triangle(std::array<Vertex, 3> _vertices,
		const glm::vec3& _ka,
		const glm::vec3& _kd,
		const glm::vec3& _ks,
		const glm::vec3& _km,
		float _n);;

	float Intersect(Ray ray, float t0, float t1) override;
	glm::vec3 getNormal(glm::vec3 position) override;
	MaterialRecord GetParameters(glm::vec3 position) override;
};
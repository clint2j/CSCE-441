#include "Triangle.h"

Triangle::Triangle(std::array<Vertex, 3> _vertices, const glm::vec3& _ka, const glm::vec3& _kd, const glm::vec3& _ks, const glm::vec3& _km, float _n)
	: Shape(_ka, _kd, _ks, _km, _n), vertices(_vertices)
{
}

float Triangle::Intersect(Ray ray, float t0, float t1)
{
	const float myZero = 0.0000001f;


	glm::vec3 vertex0 = vertices[0].pos;
	glm::vec3 vertex1 = vertices[1].pos;
	glm::vec3 vertex2 = vertices[2].pos;

	glm::vec3 edge1 = vertex1 - vertex0;
	glm::vec3 edge2 = vertex2 - vertex0;

	glm::vec3 P = glm::cross(ray.d, edge2);

	float det = glm::dot(edge1, P);

	if (det > -myZero && det < myZero)
		return INFINITY;

	float invDet = 1.0f / det;

	glm::vec3 T = ray.o - vertex0;
	float u = glm::dot(T, P) * invDet;
	if (u < 0.0f || u > 1.0f)
		return INFINITY;

	glm::vec3 Q = glm::cross(T, edge1);

	float v = glm::dot(ray.d, Q) * invDet;
	if (v < 0.0f || u + v > 1.0f)
		return INFINITY;
	float t = glm::dot(edge2, Q) * invDet;
	if (t < t0 || t > t1)
		return INFINITY;
	return t > myZero ? t : INFINITY;
}

glm::vec3 Triangle::getNormal(glm::vec3 position)
{
	glm::vec3 vertex0 = vertices[0].pos;
	glm::vec3 vertex1 = vertices[1].pos;
	glm::vec3 vertex2 = vertices[2].pos;

	glm::vec3 v0v1 = vertex1 - vertex0;
	glm::vec3 v0v2 = vertex2 - vertex0;
	glm::vec3 v0p = position - vertex0;

	float d00 = glm::dot(v0v1, v0v1);
	float d01 = glm::dot(v0v1, v0v2);
	float d11 = glm::dot(v0v2, v0v2);
	float d20 = glm::dot(v0v1, v0p);
	float d21 = glm::dot(v0v2, v0p);

	float denom = d00 * d11 - d01 * d01;
	float v = (d11 * d20 - d01 * d21) / denom;
	float w = (d00 * d21 - d01 * d20) / denom;
	float u = 1.0f - v - w;
	glm::vec3 normal = vertices[0].norm * u +
		vertices[1].norm * v +
		vertices[2].norm * w;

	return glm::normalize(normal);
}

MaterialRecord Triangle::GetParameters(glm::vec3 position)
{
	return MaterialRecord(ka, kd, ks, km, n, getNormal(position));
}
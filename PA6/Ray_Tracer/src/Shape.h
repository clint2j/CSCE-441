#pragma once
#include <glm/glm.hpp>

struct Ray {
	glm::vec3 o;
	glm::vec3 d;
	glm::vec3 getPosition(float t) {
		return t * d + o;
	}
	float getT(glm::vec3 pos) {
		return glm::dot(pos - o, d);
	}
	Ray(glm::vec3 origin, glm::vec3 distance) : o(origin), d(glm::normalize(distance)) {}
};

struct MaterialRecord {
	glm::vec3 ka = {};
	glm::vec3 kd = {};
	glm::vec3 ks = {};
	glm::vec3 km = {};
	float n = {};
	glm::vec3 normal = {};
	MaterialRecord() {};
	MaterialRecord(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, glm::vec3 mirror, float shininess, glm::vec3 _normal);
};

class Shape
{
public:
	Shape(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, glm::vec3 mirror, float shininess);
	~Shape();
	virtual float Intersect(Ray ray, float t0, float t1) = 0;
	virtual glm::vec3 getNormal(glm::vec3 position) = 0;
	virtual MaterialRecord GetParameters(glm::vec3 position) = 0;
protected:
	glm::vec3 ka, kd, ks, km;
	float n;
private:

};


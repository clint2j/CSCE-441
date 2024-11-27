#pragma once
#include <glm/glm.hpp>

//class Light
//{
//public:
//	Light(glm::vec);
//	~Light();
//
//private:
//
//};

struct Light {
	Light(glm::vec3& p, glm::vec3& c) :position(p), color(c) {};
	glm::vec3 position;
	glm::vec3 color;
};

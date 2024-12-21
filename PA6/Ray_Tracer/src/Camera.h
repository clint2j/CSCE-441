#pragma once
#include "Camera.h"
#include "Scene.h"
#include "Shape.h"

#include <glm/glm.hpp>
#include <optional>
#include <random>
#include <vector>
class Camera
{
public:
	Camera(int _widthRes, int _heightRes,
		const glm::vec3& _eye,
		const glm::vec3& _lookAt,
		const glm::vec3& _up,
		float _fovY,
		float _focalDistance,
		float aliasDistance,
		float _softShadowDistance);
	~Camera();
	void TakePicture(Scene* scene);
	float* GetRenderedImage() { return renderedImage; };
private:
	int widthRes;
	int heightRes;

	glm::vec3 eye;
	glm::vec3 lookAt;
	glm::vec3 up;
	float fovY;
	float focalDistance;

	std::mt19937 randGen32;
	std::uniform_real_distribution<float> aliasDistro;
	std::uniform_real_distribution<float> softShadowDistro;

	float* renderedImage;
	glm::vec3 ComputeRayColor(Ray ray, float t0, float t1, Scene* scene, int recursiveDepth);
};
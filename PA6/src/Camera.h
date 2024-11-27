#pragma once
#include "Camera.h"
#include "Scene.h"
#include "Shape.h"

#include <glm/glm.hpp>
#include <optional>
#include <vector>
class Camera
{
public:
	Camera(int _widthRes, int _heightRes,
		const glm::vec3& _eye,
		const glm::vec3& _lookAt,
		const glm::vec3& _up,
		float _fovY,
		float _focalDistance);
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

	float* renderedImage;
	std::optional<glm::vec3> ComputeRayColor(Ray ray, float t0, float t1, Scene* scene);
};
#include "Camera.h"
#include <iostream>

constexpr float myINF = 5000000;
constexpr float myZero = 0.00001;
//Camera::Camera(int _widthRes, int _heightRes, const glm::vec3& _eye, const glm::vec3& _lookAt, const glm::vec3& _up, float _fovY, float _focalDistance)
//{
//}

Camera::~Camera()
{
}

Camera::Camera(int _widthRes, int _heightRes,
	const glm::vec3& _eye,
	const glm::vec3& _lookAt,
	const glm::vec3& _up,
	float _fovY,
	float _focalDistance)
	: widthRes(_widthRes)
	, heightRes(_heightRes)
	, eye(_eye)
	, lookAt(_lookAt)
	, up(_up)
	, fovY(_fovY)
	, focalDistance(_focalDistance)
{
	// Allocate memory for the rendered image
	// Size is width * height * 3 for RGB values
	renderedImage = new float[widthRes * heightRes * 3];
}

void Camera::TakePicture(Scene* scene)
{
	memset(renderedImage, 0, sizeof(float) * widthRes * heightRes * 3);

	float imageHeight = 2.0f * focalDistance * tan(fovY / 2.0f);
	float imageWidth = imageHeight * (float(widthRes) / float(heightRes));

	for (int i = 0; i < widthRes; ++i) {
		for (int j = 0; j < heightRes; ++j) {
			float u = (2.0f * i - widthRes) / widthRes;
			float v = (2.0f * j - heightRes) / heightRes;
			float x = u * (imageWidth / 2.0f);
			float y = v * (imageHeight / 2.0f);
			glm::vec3 rayDirection = glm::normalize(glm::vec3(x, y, -focalDistance));
			Ray ray(eye, rayDirection);
			auto rayColor = ComputeRayColor(ray, 0, myINF, scene, 0);
			int index = (j * widthRes + i) * 3;
			renderedImage[index] = rayColor.r;
			renderedImage[index + 1] = rayColor.g;
			renderedImage[index + 2] = rayColor.b;
		}
	}
}

glm::vec3 Camera::ComputeRayColor(Ray ray, float t0, float t1, Scene* scene, int recursiveDepth)
{
	if (recursiveDepth > 4) {
		return glm::vec3(0, 0, 0);
	}
	MaterialRecord rec;
	glm::vec3 color{ 0,0,0 };
	float t = scene->Hit(ray, t0, t1, rec);
	if (t != INFINITY) {
		auto [ka, kd, ks, km, n, normal] = rec;
		glm::vec3 position = ray.getPosition(t);
		color = ka;
		auto lights = scene->getLights();
		MaterialRecord srec;
		for (Light* l : lights) {
			glm::vec3 Li = glm::normalize(l->position - position);
			Ray sray = Ray(position, Li);
			float t_shadow = scene->Hit(sray, myZero, sray.getT(l->position), srec);
			if (t_shadow == INFINITY) {
				glm::vec3 diffuse = kd * std::max(0.0f, glm::dot(Li, normal));
				glm::vec3 E = glm::normalize(eye - position);
				glm::vec3 Ri = glm::normalize(2.0f * glm::dot(Li, normal) * normal - Li);
				glm::vec3 specular = ks * std::pow(std::max(0.0f, glm::dot(Ri, E)), n);
				color += l->color * (diffuse + specular);
			}
		}
		glm::vec3 V = -1.0f * ray.d;
		glm::vec3 R = glm::normalize(2.0f * glm::dot(V, normal) * normal - V);
		Ray reflectedRay(position + normal * 0.001f, R);
		color += km * ComputeRayColor(reflectedRay, myZero, myINF, scene, recursiveDepth + 1);
	}
	return color;
}

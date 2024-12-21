#include "Camera.h"

#include <cmath>
#include <iostream>

constexpr float myINF = 500000000;
constexpr float myZero = 0.00001;
constexpr int numAliasRays = 5;
constexpr int numShadowRays = 6;

Camera::~Camera()
{
}

Camera::Camera(int _widthRes, int _heightRes,
	const glm::vec3& _eye,
	const glm::vec3& _lookAt,
	const glm::vec3& _up,
	float _fovY,
	float _focalDistance,
	float aliasDistance,
	float _softShadowDistance
)
	: widthRes(_widthRes)
	, heightRes(_heightRes)
	, eye(_eye)
	, lookAt(_lookAt)
	, up(_up)
	, fovY(_fovY)
	, focalDistance(_focalDistance)
	, aliasDistro(-1.0f * aliasDistance, aliasDistance)
	, softShadowDistro(-1.0f * _softShadowDistance, _softShadowDistance)
{
	// Allocate memory for the rendered image
	// Size is width * height * 3 for RGB values
	renderedImage = new float[widthRes * heightRes * 3];
	std::random_device rd;
	randGen32 = std::mt19937(rd());
	//std::cout << aliasDistance << '\n';
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
			auto totalRayColor = glm::vec3(0);
			for (int c = 0; c < numAliasRays; ++c) {
				float offset = aliasDistro(randGen32);
				glm::vec3 rayDirection = glm::normalize(glm::vec3(x + offset, y + offset, -focalDistance));
				Ray ray(eye, rayDirection);
				auto currRayColor = ComputeRayColor(ray, 0, myINF, scene, 0);
				totalRayColor += currRayColor;
			}
			glm::vec3 rayColor = totalRayColor * (1.0f / numAliasRays);
			int index = (j * widthRes + i) * 3;
			renderedImage[index] = rayColor.r;
			renderedImage[index + 1] = rayColor.g;
			renderedImage[index + 2] = rayColor.b;
		}
	}
}

glm::vec3 Camera::ComputeRayColor(Ray ray, float t0, float t1, Scene* scene, int recursiveDepth)
{
	if (recursiveDepth > 12) {
		return glm::vec3(0, 0, 0);
	}
	MaterialRecord rec;
	glm::vec3 color{ 0,0,0 };
	float t = scene->Hit(ray, t0, t1, rec);
	if (t != INFINITY) {
		auto [ka, kd, ks, km, kt, n, ior, normal] = rec;
		glm::vec3 position = ray.getPosition(t);
		color = ka;
		auto lights = scene->getLights();
		MaterialRecord srec;
		for (Light* l : lights) {
			glm::vec3 Li = glm::normalize(l->position - position);
			float missCount = 0;
			for (int c = 0; c < numShadowRays; ++c) {
				glm::vec3 offLightPos = { Li.x + softShadowDistro(randGen32), Li.y + softShadowDistro(randGen32) , Li.z + softShadowDistro(randGen32) };
				Ray sray = Ray(position, offLightPos);
				float t_shadow = scene->Hit(sray, myZero, sray.getT(l->position), srec);
				if (t_shadow == INFINITY)
					missCount += 1.0f;
			}
			float missPercent = missCount / numShadowRays;
			if (missPercent > 0) {
				glm::vec3 diffuse = kd * std::max(0.0f, glm::dot(Li, normal));
				glm::vec3 E = glm::normalize(eye - position);
				glm::vec3 Ri = glm::normalize(2.0f * glm::dot(Li, normal) * normal - Li);
				glm::vec3 specular = ks * std::pow(std::max(0.0f, glm::dot(Ri, E)), n);
				color += missPercent * l->color * (diffuse + specular);
			}
		}
		glm::vec3 V = -1.0f * ray.d;

		if (km.x > 0 || km.y > 0 || km.z > 0) {
			glm::vec3 R = glm::normalize(2.0f * glm::dot(V, normal) * normal - V);
			Ray reflectedRay(position + normal * 0.001f, R);
			color += km * ComputeRayColor(reflectedRay, myZero, myINF, scene, recursiveDepth + 1);
		}

		if (kt.x > 0 || kt.y > 0 || kt.z > 0) {
			float n1 = ray.inside ? ior : 1.0f;
			float n2 = ray.inside ? 1.0f : ior;
			float eta = n1 / n2;
			if (ray.inside) {
				normal = -normal;
			}
			float cosI = glm::dot(-V, normal);
			float sinT2 = eta * eta * (1.0f - cosI * cosI);
			if (sinT2 <= 1.0f) {
				if (ior == 1.0f) {
					Ray refractedRay(ray.getPosition(t + 2), ray.d);
					refractedRay.inside = !ray.inside;
					color += kt * ComputeRayColor(refractedRay, myZero, myINF, scene, recursiveDepth + 1);
				}
			}
		}
	}
	return color;
}

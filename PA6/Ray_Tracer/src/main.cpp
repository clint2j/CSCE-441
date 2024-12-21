#define GLEW_STATIC
#include "Camera.h"
#include "Plane.h"
#include "Scene.h"
#include "Sphere.h"
#include "Triangle.h"

#include <algorithm>
#include <array>
#include <chrono>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <memory>
#include <random>
#include <vector>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"


constexpr auto WINDOW_HEIGHT = 800;
constexpr auto WINDOW_WIDTH = 1200;
using Frame_Buf = float[WINDOW_HEIGHT][WINDOW_WIDTH][3];

GLFWwindow* window;

std::array< Frame_Buf, 2> framesRendered;
int frameIndex = 0;
bool frame2Loaded = false;

std::vector<Shape*> shapes;
std::vector<Light*> lights;

std::vector<float> posBuff;
std::vector<float> norBuff;
std::vector<float> texBuff;

std::string resource_path = "";

//std::vector<Triangle> tris;

std::size_t numTriangles = 1000;

float aliasDistance = 0.0001;

float softShadowRadius = .004;

bool refractionSphere = false;

void ClearFrameBuffers()
{
	for (auto& frame : framesRendered)
		memset(frame, 0, WINDOW_HEIGHT * WINDOW_WIDTH * 3 * sizeof(float));
}

void Display()
{
	glDrawPixels(WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGB, GL_FLOAT, framesRendered[frameIndex]);
}

float* renderScene() {
	std::vector<Shape*> shapes;
	if (!refractionSphere) {
		shapes.push_back(new Sphere(
			glm::vec3(-1.0f, -0.7f, 3.0f),
			0.45f,
			glm::vec3(0.1f, 0.1f, 0.1f),
			glm::vec3(0.2f, 1.0f, 0.2f),
			glm::vec3(1.0f, 1.0f, 1.0f),
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 0.0f, 0.0f),
			100.0f,
			1
		));
	}
	else {
		shapes.push_back(new Sphere(
			glm::vec3(-1.0f, -0.7f, 3.0f),
			0.45f,
			glm::vec3(0.1f, 0.1f, 0.1f),
			glm::vec3(0),
			glm::vec3(0),
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(1),
			100.0f,
			1
		));
	}


	shapes.push_back(new Sphere(
		glm::vec3(1.0f, -0.5f, 3.0f),
		0.5f,
		glm::vec3(0.1f, 0.1f, 0.1f),
		glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		10.0f,
		1
	));

	shapes.push_back(new Sphere(
		glm::vec3(-1.0f, 0.0f, 0.0f),
		1.0f,
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		0.0f,
		1
	));

	shapes.push_back(new Sphere(
		glm::vec3(1.0f, 0.0f, -1.0f),
		1.0f,
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.8f, 0.8f, 0.8f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		0.0f,
		1
	));
	shapes.push_back(new Plane(
		glm::vec3(0.0f, -1.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(0.1f, 0.1f, 0.1f),
		glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		0.0f,
		1
	));

	if (!refractionSphere) {

		shapes.push_back(new Plane(
			glm::vec3(0.0f, 0.0f, -3.0f),
			glm::vec3(0.0f, 0.0f, 1.0f),
			glm::vec3(0.1f, 0.1f, 0.1f),
			glm::vec3(1.0f, 1.0f, 1.0f),
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 0.0f, 0.0f),
			0.0f,
			1
		));
	}
	else {
		shapes.push_back(new Plane(
			glm::vec3(0.0f, 0.0f, -3.0f),
			glm::vec3(0.0f, 0.0f, 1.0f),
			glm::vec3(0.1f, 0.8f, 0.6f),
			glm::vec3(1.0f, 1.0f, 1.0f),
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.4f, 0.4f, 0.4f),
			glm::vec3(0.0f, 0.0f, 0.0f),
			0.0f,
			1
		));
	}



	shapes.push_back(new Plane(
		glm::vec3(0.0f, 0.0f, -3.0f),
		glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(0.1f, 0.1f, 0.1f),
		glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		0.0f,
		1
	));

	std::vector<Light*> lights;
	lights.push_back(new Light(glm::vec3(0, 3, -2), glm::vec3(.2, .2, .2)));
	lights.push_back(new Light(glm::vec3(-2, 1, 4), glm::vec3(.5, .5, .5)));
	Scene scene = Scene(std::move(shapes), std::move(lights));
	Camera camera = Camera(
		1200,
		800,
		glm::vec3(0.0f, 0.0f, 7.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		45.0f,
		1.0f,
		aliasDistance,
		softShadowRadius
	);

	auto start = std::chrono::high_resolution_clock::now();
	camera.TakePicture(&scene);
	auto stop = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
	std::cout << "Time passed for scene 1 (ms): " << duration.count() << std::endl;

	return camera.GetRenderedImage();
}

static void CharacterCallback(GLFWwindow* lWindow, unsigned int key)
{
	bool reRender = false;
	switch (key) {

	case 'A':
		aliasDistance += 0.0005;
		memcpy(framesRendered[0], renderScene(), sizeof(float) * WINDOW_HEIGHT * WINDOW_WIDTH * 3);
		break;
	case 'a':
		aliasDistance -= 0.0005;
		if (aliasDistance < 0) {
			aliasDistance = 0;
		}
		memcpy(framesRendered[0], renderScene(), sizeof(float) * WINDOW_HEIGHT * WINDOW_WIDTH * 3);
		break;
	case 'S':
		softShadowRadius += .003;
		memcpy(framesRendered[0], renderScene(), sizeof(float) * WINDOW_HEIGHT * WINDOW_WIDTH * 3);
		break;
	case 's':
		softShadowRadius -= .003;
		if (softShadowRadius < 0) {
			softShadowRadius = 0;
		}
		memcpy(framesRendered[0], renderScene(), sizeof(float) * WINDOW_HEIGHT * WINDOW_WIDTH * 3);
		break;
	case 'R':
		refractionSphere = true;
		memcpy(framesRendered[0], renderScene(), sizeof(float) * WINDOW_HEIGHT * WINDOW_WIDTH * 3);
		break;
	case 'r':
		refractionSphere = false;
		memcpy(framesRendered[0], renderScene(), sizeof(float) * WINDOW_HEIGHT * WINDOW_WIDTH * 3);
		break;
	default:
		break;
	}
}



void Init()
{
	glfwInit();
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Assignment6 - Clint Hart", NULL, NULL);
	glfwMakeContextCurrent(window);
	glewExperimental = GL_TRUE;
	glewInit();

	glfwSetCharCallback(window, CharacterCallback);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	ClearFrameBuffers();

	float* renderedImage = renderScene();
	memcpy(framesRendered[0], renderedImage, sizeof(float) * WINDOW_HEIGHT * WINDOW_WIDTH * 3);
}


int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cout << "Need to specify the resource path as the input argument." << std::endl;
		return 0;
	}
	else
		resource_path = argv[1];
	Init();
	while (glfwWindowShouldClose(window) == 0)
	{
		glClear(GL_COLOR_BUFFER_BIT);
		Display();
		glFlush();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}
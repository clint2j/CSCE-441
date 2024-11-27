#define GLEW_STATIC
#include "Camera.h"
#include "Plane.h"
#include "Scene.h"
#include "Sphere.h"

#include <array>
#include <chrono>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <random>
#include <vector>


constexpr auto WINDOW_HEIGHT = 800;
constexpr auto WINDOW_WIDTH = 1200;
using Frame_Buf = float[WINDOW_HEIGHT][WINDOW_WIDTH][3];

GLFWwindow* window;

std::array< Frame_Buf, 2> framesRendered;
int frameIndex = 0;
bool frame2Loaded = false;

std::vector<Shape*> shapes;
std::vector<Light*> lights;

void ClearFrameBuffers()
{
	for (auto& frame : framesRendered)
		memset(frame, 0, WINDOW_HEIGHT * WINDOW_WIDTH * 3 * sizeof(float));
}

void initFrame2();

void Display()
{
	if (frameIndex == 1 && !frame2Loaded) {
		initFrame2();
		frame2Loaded = true;
		std::cout << "Frame 2 loaded lol\n";
	}
	glDrawPixels(WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGB, GL_FLOAT, framesRendered[frameIndex]);
}

static void CharacterCallback(GLFWwindow* lWindow, unsigned int key)
{
	switch (key) {

	case '1':
		frameIndex = 0;
		break;
	case '2':
		frameIndex = 1;
		break;
	default:
		break;
	}
}

void initFrame2() {
	std::random_device rd;
	std::mt19937_64 gen64{ rd() };
	std::uniform_int_distribution<> distrib(1, 10);
	for (int i = 0; i < WINDOW_HEIGHT; ++i) {
		for (int j = 0; j < WINDOW_WIDTH; ++j) {
			framesRendered[1][i][j][0] = .6;
			framesRendered[1][i][j][1] = .9;
			framesRendered[1][i][j][2] = .2;
			if (distrib(gen64) <= 2) {
				framesRendered[1][i][j][0] = .4;
				framesRendered[1][i][j][1] = .1;
				framesRendered[1][i][j][2] = .8;
			}
		}
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

	std::vector<Shape*> shapes;
	shapes.push_back(new Sphere(
		glm::vec3(-1.0f, -0.7f, 3.0f),
		0.3f,
		glm::vec3(0.1f, 0.1f, 0.1f),
		glm::vec3(0.2f, 1.0f, 0.2f),
		glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		100.0f
	));

	shapes.push_back(new Sphere(
		glm::vec3(1.0f, -0.5f, 3.0f),
		0.5f,
		glm::vec3(0.1f, 0.1f, 0.1f),
		glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		10.0f
	));

	shapes.push_back(new Sphere(
		glm::vec3(-1.0f, 0.0f, 0.0f),
		1.0f,
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(1.0f, 1.0f, 1.0f),
		0.0f
	));

	shapes.push_back(new Sphere(
		glm::vec3(1.0f, 0.0f, -1.0f),
		1.0f,
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.8f, 0.8f, 0.8f),
		0.0f
	));

	shapes.push_back(new Plane(
		glm::vec3(0.0f, -1.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(0.1f, 0.1f, 0.1f),
		glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		0.0f
	));

	shapes.push_back(new Plane(
		glm::vec3(0.0f, 0.0f, -3.0f),
		glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(0.1f, 0.1f, 0.1f),
		glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		0.0f
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
		1.0f
	);

	auto start = std::chrono::high_resolution_clock::now();
	camera.TakePicture(&scene);
	auto stop = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
	std::cout << "Time passed for scene 1 (ms): " << duration.count() << std::endl;

	float* renderedImage = camera.GetRenderedImage();
	memcpy(framesRendered[0], renderedImage, sizeof(float) * WINDOW_HEIGHT * WINDOW_WIDTH * 3);
}


int main()
{
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
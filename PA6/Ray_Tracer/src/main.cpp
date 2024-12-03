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

void ClearFrameBuffers()
{
	for (auto& frame : framesRendered)
		memset(frame, 0, WINDOW_HEIGHT * WINDOW_WIDTH * 3 * sizeof(float));
}

void CreateTriangleVector(std::vector<glm::vec3>& vertices, std::vector<glm::vec3> normals, glm::vec3 ka, glm::vec3 kd, glm::vec3 ks, glm::vec3 km, float n)
{
	for (std::size_t i = 0; i < std::min(vertices.size(), numTriangles) / 3; i++) {
		//for (std::size_t i = 0; i < 60 / 3; ++i) {
		std::array <Vertex, 3> tri;
		for (int j = 0; j < 3; ++j) {
			tri[j].pos = vertices[i * 3 + j];
			tri[j].norm = normals[i * 3 + j];
		}
		Shape* myTriangle = new Triangle(tri, ka, kd, ks, km, n);
		shapes.push_back(myTriangle);
	}
}

void LoadModel(const std::string& name, std::vector<glm::vec3>& vertices, std::vector<glm::vec3>& normals)
{
	// Taken from Shinjiro Sueda with slight modification
	std::string meshName(name);
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string errStr;
	bool rc = tinyobj::LoadObj(&attrib, &shapes, &materials, &errStr, meshName.c_str());
	if (!rc) {
		std::cerr << errStr << std::endl;
	}
	else {
		// Some OBJ files have different indices for vertex positions, normals,
		// and texture coordinates. For example, a cube corner vertex may have
		// three different normals. Here, we are going to duplicate all such
		// vertices.
		// Loop over shapes
		for (size_t s = 0; s < shapes.size(); s++) {
			// Loop over faces (polygons)
			size_t index_offset = 0;
			for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
				size_t fv = shapes[s].mesh.num_face_vertices[f];
				// Loop over vertices in the face.
				for (size_t v = 0; v < fv; v++) {
					// access to vertex
					tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
					vertices.push_back(glm::vec3(attrib.vertices[3 * idx.vertex_index + 0],
						attrib.vertices[3 * idx.vertex_index + 1],
						attrib.vertices[3 * idx.vertex_index + 2]));
					normals.push_back(glm::vec3(attrib.normals[3 * idx.vertex_index + 0],
						attrib.normals[3 * idx.vertex_index + 1],
						attrib.normals[3 * idx.vertex_index + 2]));
				}
				index_offset += fv;
			}
		}
	}
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
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	LoadModel(resource_path + "/bunny.obj", vertices, normals);
	shapes.clear();
	CreateTriangleVector(vertices, normals, glm::vec3(.1, .1, .1), glm::vec3(0, 0, 1.0), glm::vec3(1, 1, .5), glm::vec3(0, 0, 0), 100.0f);
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
	std::cout << "Time passed for scene 2 (ms): " << duration.count() << std::endl;

	float* renderedImage = camera.GetRenderedImage();
	memcpy(framesRendered[1], renderedImage, sizeof(float) * WINDOW_HEIGHT * WINDOW_WIDTH * 3);
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
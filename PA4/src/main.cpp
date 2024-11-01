#define GLEW_STATIC
#include <algorithm>
#include <array>
#include <fstream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "Program.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"


#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define NUM_LIGHTS 2
#define NUM_MATERIALS 3

enum ShadingMode { Gourand, Phong, Silhouette };

ShadingMode mode = ShadingMode::Gourand;

constexpr float LIST_MOVE = 2.0f;

unsigned int currMaterial = 0;

unsigned int currLightToEdit = 0;

bool spotlightOn = false;

GLFWwindow* window;
std::string resource_path = "";

//Program program;

std::array<Program, 3> Programs;

std::vector<float> posBuff;
std::vector<float> norBuff;
std::vector<float> texBuff;

glm::vec3 eye(0.0f, 0.0f, 4.0f);

struct materialStruct {
	glm::vec3 ka;  // ambient coefficient
	glm::vec3 kd;  // diffuse coefficient
	glm::vec3 ks;  // specular coefficient
	float s;       // specular exponent
};

struct lightStruct {
	glm::vec3 position;
	glm::vec3 color;
};

std::array<materialStruct, NUM_MATERIALS> materials;
std::array<lightStruct, NUM_LIGHTS> lights;


void Display()
{
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);

	glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f), float(width) / float(height), 0.1f, 100.0f);
	glm::mat4 viewMatrix = glm::lookAt(eye, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	glm::mat4 modelMatrix(1.0f);
	modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.2f, -1.0f, 0.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	auto& program = Programs[mode];
	program.Bind();
	program.SendUniformData(modelMatrix, "model");
	program.SendUniformData(viewMatrix, "view");
	program.SendUniformData(projectionMatrix, "projection");

	program.SendUniformData(lights[0].position, "lights[0].position");
	program.SendUniformData(lights[0].color, "lights[0].color");
	program.SendUniformData(lights[1].position, "lights[1].position");
	program.SendUniformData(lights[1].color, "lights[1].color");

	program.SendUniformData(materials[currMaterial].ka, "ka");
	program.SendUniformData(materials[currMaterial].kd, "kd");
	program.SendUniformData(materials[currMaterial].ks, "ks");
	program.SendUniformData(materials[currMaterial].s, "s");

	glDrawArrays(GL_TRIANGLES, 0, posBuff.size() / 3);
	program.Unbind();

}

// Keyboard character callback function
void CharacterCallback(GLFWwindow* lWindow, unsigned int key)
{
	switch (key)
	{
	case 'q':
		glfwSetWindowShouldClose(window, GLFW_TRUE);
		break;
	case 'm':
		++currMaterial;
		currMaterial %= NUM_MATERIALS;
		break;
	case 'M':
		if (currMaterial != 0)
			--currMaterial;
		else
			currMaterial = NUM_MATERIALS - 1;
		break;
	case '1':
		mode = ShadingMode::Gourand;
		break;
	case '2':
		mode = ShadingMode::Phong;
		break;
	case '3':
		mode = ShadingMode::Silhouette;
		break;
	case 'l':
		++currLightToEdit;
		currLightToEdit %= NUM_LIGHTS;
		break;
	case 'L':
		if (currLightToEdit != 0)
			--currLightToEdit;
		else
			currLightToEdit = NUM_LIGHTS - 1;
		break;
	case 'x':
		std::cout << "Bruh Lmao " << lights[currLightToEdit].position.x << "\n";
		lights[currLightToEdit].position.x += LIST_MOVE;
		break;
	case 'X':
		lights[currLightToEdit].position.x -= LIST_MOVE;
		break;
	case 'y':
		lights[currLightToEdit].position.y += LIST_MOVE;
		break;
	case 'Y':
		lights[currLightToEdit].position.y -= LIST_MOVE;
		break;
	case 'z':
		lights[currLightToEdit].position.z += LIST_MOVE;
		break;
	case 'Z':
		lights[currLightToEdit].position.z -= LIST_MOVE;
		break;
	default:
		break;
	}
}


void FrameBufferSizeCallback(GLFWwindow* lWindow, int width, int height)
{
	glViewport(0, 0, width, height);
}

void setInitalLightAndMaterials() {
	materials[0].ka = { .2,.2,.2 };
	materials[0].kd = { .8,.7,.7 };
	materials[0].ks = { 1,1,1 };
	materials[0].s = 10.0;

	materials[1].ka = { 0.0, 0.2, 0.2 };
	materials[1].kd = { 0.5, 0.7, 0.2 };
	materials[1].ks = { 0.1, 1.0, 0.1 };
	materials[1].s = 100.0;

	materials[2].ka = { 0.2, 0.2, 0.2 };
	materials[2].kd = { 0.1, 0.3, 0.9 };
	materials[2].ks = { 0.1, 0.1, 0.1 };
	materials[2].s = 1.0;

	lights[0].position = { 0,0,3 };
	lights[0].color = { .5,.5,.5 };

	lights[1].position = { 0,3,0 };
	lights[1].color = { .2,.2,.2 };
}

void LoadModel(const std::string& name)
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
					posBuff.push_back(attrib.vertices[3 * idx.vertex_index + 0]);
					posBuff.push_back(attrib.vertices[3 * idx.vertex_index + 1]);
					posBuff.push_back(attrib.vertices[3 * idx.vertex_index + 2]);
					if (!attrib.normals.empty()) {
						norBuff.push_back(attrib.normals[3 * idx.normal_index + 0]);
						norBuff.push_back(attrib.normals[3 * idx.normal_index + 1]);
						norBuff.push_back(attrib.normals[3 * idx.normal_index + 2]);
					}
					if (!attrib.texcoords.empty()) {
						texBuff.push_back(attrib.texcoords[2 * idx.texcoord_index + 0]);
						texBuff.push_back(attrib.texcoords[2 * idx.texcoord_index + 1]);
					}
				}
				index_offset += fv;
				// per-face material (IGNORE)
				shapes[s].mesh.material_ids[f];
			}
		}
	}
}

void Init()
{
	glfwInit();
	glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GL_FALSE);
	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Assignment4 - Clint Hart", NULL, NULL);
	glfwMakeContextCurrent(window);
	glewExperimental = GL_TRUE;
	glewInit();
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	glfwSetCharCallback(window, CharacterCallback);
	glfwSetFramebufferSizeCallback(window, FrameBufferSizeCallback);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glEnable(GL_DEPTH_TEST);

	LoadModel(resource_path + "/bunny.obj");

	setInitalLightAndMaterials();

	Programs[0].SetShadersFileName(resource_path + "/vert.glsl", resource_path + "/frag.glsl");
	Programs[0].Init();
	Programs[0].SendAttributeData(posBuff, "vPositionModel");
	Programs[0].SendAttributeData(norBuff, "vNormalModel");

	Programs[1].SetShadersFileName(resource_path + "/vert1.glsl", resource_path + "/frag1.glsl");
	Programs[1].Init();
	Programs[1].SendAttributeData(posBuff, "vPositionModel");
	Programs[1].SendAttributeData(norBuff, "vNormalModel");

	Programs[2].SetShadersFileName(resource_path + "/vert2.glsl", resource_path + "/frag2.glsl");
	Programs[2].Init();
	Programs[2].SendAttributeData(posBuff, "vPositionModel");
	Programs[2].SendAttributeData(norBuff, "vNormalModel");
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
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		Display();
		glFlush();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}
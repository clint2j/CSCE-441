#define GLEW_STATIC
#include <algorithm>
#include <array>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>

#include "Program.h"
#include "Shape.h"



#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define NUM_LIGHTS 1
#define SHADOW_WIDTH 1024
#define SHADOW_HEIGHT 1024
#define NUM_MATERIALS 2

GLFWwindow* window;
std::string resource_path = "";

std::vector<Program> prog;

GLuint depthbuffer = 0;
GLuint shadowMap = 0;
bool isShadowMap = false;

std::vector<Shape> mainObjs;
Shape ground;

glm::vec3 eye(0.0f, 5.0f, 5.0f);

struct MaterialStruct {
	glm::vec3 ka;  // ambient coefficient
	glm::vec3 kd;  // diffuse coefficient
	glm::vec3 ks;  // specular coefficient
	float s;       // specular exponent
};

struct LightStruct {
	glm::vec3 position;
	glm::vec3 color;
};

std::array<MaterialStruct, NUM_MATERIALS> materials;
std::array<LightStruct, NUM_LIGHTS> lights;

glm::vec3 n_ground;
glm::vec3 p_ground;

static void Display()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	int width, height;
	glfwGetFramebufferSize(window, &width, &height);

	// To make sure the code runs when window is minimized
	if (width == 0 || height == 0)
		width = WINDOW_WIDTH, height = WINDOW_HEIGHT;


	glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f), float(width) / float(height), 0.1f, 100.0f);
	glm::mat4 viewMatrix = glm::lookAt(eye, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));


	if (!isShadowMap)
	{
		//******************************************************//
		//********* Planar Shadow Implementation ***************//
		//******************************************************//

		glViewport(0, 0, width, height);
		prog[0].Bind();

		prog[0].SendUniformData(viewMatrix, "view");
		prog[0].SendUniformData(projectionMatrix, "projection");

		//rendering data
		prog[0].SendUniformData(lights[0].position, "lights[0].position");
		prog[0].SendUniformData(lights[0].color, "lights[0].color");

		prog[0].SendUniformData(materials[0].ka, "ka");
		prog[0].SendUniformData(materials[0].kd, "kd");
		prog[0].SendUniformData(materials[0].ks, "ks");
		prog[0].SendUniformData(materials[0].s, "s");
		//prog[0].SendUniformData(eye, "eye");

		for (int i = 0; i < mainObjs.size(); i++)
		{
			prog[0].SendUniformData(mainObjs[i].GetModelMatrix(), "model");
			mainObjs[i].Draw(prog[0]);
		}

		/// Commented out so you can see the red Bunny and Teapot. 
		/// Otherwise, the whole screen will be rendered red because of the ground.
		/// Uncomment once you start implementing the approach.
		prog[0].SendUniformData(materials[1].ka, "ka");
		prog[0].SendUniformData(materials[1].kd, "kd");
		prog[0].SendUniformData(materials[1].ks, "ks");
		prog[0].SendUniformData(materials[1].s, "s");

		prog[0].SendUniformData(ground.GetModelMatrix(), "model");
		ground.Draw(prog[0]);

		prog[0].Unbind();

		//Shadow time!
		prog[1].Bind();
		prog[1].SendUniformData(viewMatrix, "view");
		prog[1].SendUniformData(projectionMatrix, "projection");
		prog[1].SendUniformData(lights[0].position, "lights[0].position");
		prog[1].SendUniformData(lights[0].color, "lights[0].color");
		prog[1].SendUniformData(eye, "eye");
		prog[1].SendUniformData(n_ground, "n_ground");
		prog[1].SendUniformData(p_ground, "p_ground");


		for (int i = 0; i < mainObjs.size(); i++)
		{
			prog[1].SendUniformData(mainObjs[i].GetModelMatrix(), "model");
			mainObjs[i].Draw(prog[1]);
		}
		prog[1].Unbind();
	}
	else
	{
		//******************************************************//
		//********* Shadow Map Implementation ******************//
		//******************************************************//

		// 1. Render to depth map
		glBindFramebuffer(GL_FRAMEBUFFER, depthbuffer);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);


		prog[2].Bind();

		prog[2].SendUniformData(viewMatrix, "view");
		prog[2].SendUniformData(projectionMatrix, "projection");

		prog[2].SendUniformData(lights[0].position, "lights[0].position");
		prog[2].SendUniformData(lights[0].color, "lights[0].color");

		for (int i = 0; i < mainObjs.size(); i++)
		{
			prog[2].SendUniformData(mainObjs[i].GetModelMatrix(), "model");
			mainObjs[i].Draw(prog[2]);
		}

		//prog[2].SendUniformData(ground.GetModelMatrix(), "model");
		//ground.Draw(prog[2]);

		prog[2].Unbind();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// 2. Render scene as normal with shadow mapping (using depth map)
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, width, height);


		prog[3].Bind();



		/// Passing the shadowMap to the shader
		int unit = 0;
		glActiveTexture(GL_TEXTURE0 + unit);
		glBindTexture(GL_TEXTURE_2D, shadowMap);
		glUniform1i(glGetUniformLocation(prog[3].GetPID(), "shadowMap"), unit);



		prog[3].SendUniformData(viewMatrix, "view");
		prog[3].SendUniformData(projectionMatrix, "projection");

		prog[3].SendUniformData(lights[0].position, "lights[0].position");
		prog[3].SendUniformData(lights[0].color, "lights[0].color");

		prog[3].SendUniformData(materials[0].ka, "ka");
		prog[3].SendUniformData(materials[0].kd, "kd");
		prog[3].SendUniformData(materials[0].ks, "ks");
		prog[3].SendUniformData(materials[0].s, "s");

		for (int i = 0; i < mainObjs.size(); ++i)
		{
			prog[3].SendUniformData(mainObjs[i].GetModelMatrix(), "model");
			mainObjs[i].Draw(prog[3]);
		}

		/// Commented out so you can see the red Bunny and Teapot. 
		/// Otherwise, the whole screen will be rendered red because of the ground.
		/// Uncomment once you start implementing the approach.

		prog[3].SendUniformData(materials[1].ka, "ka");
		prog[3].SendUniformData(materials[1].kd, "kd");
		prog[3].SendUniformData(materials[1].ks, "ks");
		prog[3].SendUniformData(materials[1].s, "s");

		prog[3].SendUniformData(ground.GetModelMatrix(), "model");
		ground.Draw(prog[3]);

		prog[3].Unbind();


		/// 3. Debugging tool to make sure the depth map is obtained properly.
		/// Will be shown at the bottom left corner of the display window.
		/// DO NOT TOUCH THIS PART!!
		glDisable(GL_DEPTH_TEST);
		glUseProgram(0);
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glOrtho(0, 1, 0, 1, 1, 3);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
		glColor4f(1, 1, 1, 1);
		glTranslated(0, 0, -2.0);
		glActiveTextureARB(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, shadowMap);
		glEnable(GL_TEXTURE_2D);

		glBegin(GL_QUADS);
		float sy = 0.25;
		float sx = sy / (float(SHADOW_WIDTH) / float(SHADOW_HEIGHT));
		glTexCoord2d(0, 0); glVertex3f(0, 0, 0);
		glTexCoord2d(1, 0); glVertex3f(sx, 0, 0);
		glTexCoord2d(1, 1); glVertex3f(sx, sy, 0);
		glTexCoord2d(0, 1); glVertex3f(0, sy, 0);
		glEnd();

		glDisable(GL_TEXTURE_2D);
		glPopMatrix();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
	}

	glFlush();
}

// Keyboard character callback function
static void CharacterCallback(GLFWwindow* lWindow, unsigned int key)
{
	switch (key) {

	case 's':
		isShadowMap = !isShadowMap;
		break;
	case 'x':
		lights[0].position.x += 0.05;
		break;
	case 'y':
		lights[0].position.y += 0.05;
		break;
	case 'z':
		lights[0].position.z += 0.05;
		break;
	case 'X':
		lights[0].position.x -= 0.05;
		break;
	case 'Y':
		lights[0].position.y -= 0.05;
		break;
	case 'Z':
		lights[0].position.z -= 0.05;
		break;
	case 'q':
		glfwSetWindowShouldClose(window, GLFW_TRUE);
		break;
	default:
		break;
	}
}

static void setInitalLightAndMaterials() {
	materials[0].ka = { .2,.2,.2 };
	materials[0].kd = { .8,.7,.7 };
	materials[0].ks = { 1,1,1 };
	materials[0].s = 10.0;

	materials[1].ka = { 0.2, 0.2, 0.2 };
	materials[1].kd = { 0.8, 0.1, 0.1 };
	materials[1].ks = { 1.0, 1.0, 1.0 };
	materials[1].s = 100.0;

	lights[0].position = { 0,5,1 };
	lights[0].color = { .5,.5,.5 };

	n_ground = { 0,1,0 };
	p_ground = { 0,0,0 };
}

static void Init()
{
	glfwInit();
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GL_FALSE);
	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Assignment4 - Clint Hart", NULL, NULL);
	glfwMakeContextCurrent(window);
	glewExperimental = GL_TRUE;
	glewInit();
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	glfwSetCharCallback(window, CharacterCallback);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glEnable(GL_DEPTH_TEST);


	/// Loading the objects
	mainObjs.push_back(Shape());
	mainObjs.push_back(Shape());

	mainObjs[0].LoadModel(resource_path + "/teapot.obj");
	mainObjs[0].Init();
	mainObjs[0].SetModel(glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, 0.0f, 0.0f)));

	mainObjs[1].LoadModel(resource_path + "/bunny.obj");
	mainObjs[1].Init();
	mainObjs[1].SetModel(glm::translate(glm::mat4(1.0f), glm::vec3(-0.2f, 0.0f, 1.3f)));

	ground.LoadModel(resource_path + "/square.obj");
	ground.Init();
	ground.SetModel(glm::scale(glm::mat4(1.0f), glm::vec3(20.0f)) *
		glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)));

	setInitalLightAndMaterials();
	/// Setting up the shader programs
	prog.push_back(Program());
	prog[0].SetShadersFileName(resource_path + "/planar_render_vert.glsl", resource_path + "/planar_render_frag.glsl");
	prog[0].Init();

	prog.push_back(Program());
	prog[1].SetShadersFileName(resource_path + "/planar_shadow_vert.glsl", resource_path + "/planar_shadow_frag.glsl");
	prog[1].Init();

	prog.push_back(Program());
	prog[2].SetShadersFileName(resource_path + "/map_pass1_vert.glsl", resource_path + "/map_pass1_frag.glsl");
	prog[2].Init();

	prog.push_back(Program());
	prog[3].SetShadersFileName(resource_path + "/map_pass2_vert.glsl", resource_path + "/map_pass2_frag.glsl");
	prog[3].Init();

	// Initialize frame buffers	
	glGenTextures(1, &shadowMap);
	glBindTexture(GL_TEXTURE_2D, shadowMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glGenFramebuffers(1, &depthbuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, depthbuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap, 0);
	glDrawBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

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
		Display();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}
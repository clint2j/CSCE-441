#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#define WINDOW_WIDTH	800
#define WINDOW_HEIGHT	600

GLFWwindow *pWindow = NULL;
GLuint vert_shader, frag_shader, vao, vbo, pid;
std::string resource_path = "";

void init()
{
	glfwInit();
	glfwSetTime(0.0);
	if (pWindow)
	{
		glfwDestroyWindow(pWindow);
	}
	pWindow = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Hello World!", NULL, NULL);
	glfwMakeContextCurrent(pWindow);
	glewExperimental = true;
	glewInit();
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
}

bool setupScene()
{
	GLint succ;
	vert_shader = glCreateShader(GL_VERTEX_SHADER);
	frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
	std::ifstream ifs;
	std::string str;
	std::stringstream ss;


	ifs.open(resource_path + "/vert.glsl");
	ss << ifs.rdbuf();
	ifs.close();
	str = ss.str();
	const char* textvs = str.c_str();
	glShaderSource(vert_shader, 1, &textvs, NULL);
	glCompileShader(vert_shader);
	glGetShaderiv(vert_shader, GL_COMPILE_STATUS, &succ);
	if (succ == GL_FALSE) {
		std::cout << "Fail to compile vertex shader!" << std::endl;
		glDeleteShader(vert_shader);
		vert_shader = 0;
		return false;
	}

	ss.str("");
	ifs.open(resource_path + "/frag.glsl");
	ss << ifs.rdbuf();
	ifs.close();
	str = ss.str();
	const char* textfs = str.c_str();
	glShaderSource(frag_shader, 1, &textfs, NULL);
	glCompileShader(frag_shader);
	glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &succ);
	if (succ == GL_FALSE) {
		std::cout << "Fail to compile fragment shader!" << std::endl;
		glDeleteShader(frag_shader);
		frag_shader = 0;
		return false;
	}
	pid = glCreateProgram();
	glAttachShader(pid, vert_shader);
	glAttachShader(pid, frag_shader);
	glLinkProgram(pid);
	glDetachShader(pid, vert_shader);
	glDetachShader(pid, frag_shader);
	glUseProgram(pid);

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	// x, y, z, r, g, b, ...
	float data_buffer[] = {
		// Face 1
		0.0,		0.0,		1.0,		1.0,	0.5,	0.5,
		0.4714,		-0.8165,	-0.3333,	1.0,	0.5,	0.5,
		0.4714,		0.8165,		-0.3333,	1.0,	0.5,	0.5,
		// Face 2
		0.0,		0.0,		1.0,		0.5,	1.0,	0.5,
		0.4714,		0.8165,		-0.3333,	0.5,	1.0,	0.5,
		-0.9428,	0.0,		-0.3333,	0.5,	1.0,	0.5,
		// Face 3
		0.0,		0.0,		1.0,		0.5,	0.5,	1.0,
		-0.9428,	0.0,		-0.3333,	0.5,	0.5,	1.0,
		0.4714,		-0.8165,	-0.3333,	0.5,	0.5,	1.0
	};

	glBufferData(GL_ARRAY_BUFFER, 3 * 3 * 6 * sizeof(float), data_buffer, GL_STATIC_DRAW);
	glEnableVertexAttribArray(glGetAttribLocation(pid, "Pos"));
	glEnableVertexAttribArray(glGetAttribLocation(pid, "Col"));

	glVertexAttribPointer(glGetAttribLocation(pid, "Pos"), 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), NULL);
	glVertexAttribPointer(glGetAttribLocation(pid, "Col"), 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glUniformMatrix4fv(glGetUniformLocation(pid, "Proj"), 1, GL_FALSE, &(glm::ortho(-1.0f * WINDOW_WIDTH / WINDOW_HEIGHT, 1.0f * WINDOW_WIDTH / WINDOW_HEIGHT, -1.0f, 1.0f, 0.0f, 100.0f)[0][0]));

	return true;
}

void display()
{
	glUseProgram(pid);
	double t = glfwGetTime();
	glUniformMatrix4fv(glGetUniformLocation(pid, "View"), 1, GL_FALSE, &(glm::lookAt(glm::vec3(10 * cos(t), 10 * sin(t), 6.66), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 1.0))[0][0]));
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 9);
	glBindVertexArray(0);
}

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		std::cout << "Need to specify the resource path as the input argument." << std::endl;
		return 0;
	}
	else
		resource_path = argv[1];

	init();
	setupScene();
	while (!glfwWindowShouldClose(pWindow)) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		display();
		glfwSwapBuffers(pWindow);
		glfwPollEvents();
	}
	return 0;
}
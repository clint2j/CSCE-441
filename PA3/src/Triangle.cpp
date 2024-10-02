#include "Triangle.h"
#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>
#include <algorithm>

// A function clamping the input values to the lower and higher bounds
#define CLAMP(in, low, high) ((in) < (low) ? (low) : ((in) > (high) ? (high) : in))


Triangle::Triangle(glm::vec3 &v0, glm::vec3 &v1, glm::vec3 &v2)
{
	v[0] = v0;
	v[1] = v1;
	v[2] = v2;

	c[0] = glm::vec3(1.0f, 1.0f, 1.0f);
	c[1] = glm::vec3(1.0f, 1.0f, 1.0f);
	c[2] = glm::vec3(1.0f, 1.0f, 1.0f);
}

// Rendering the triangle using OpenGL
void Triangle::RenderOpenGL(glm::mat4 &modelViewMatrix, glm::mat4 &projectionMatrix) 
{

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(glm::value_ptr(modelViewMatrix));

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(glm::value_ptr(projectionMatrix));

	glBegin(GL_TRIANGLES);
		glColor3f(c[0].x, c[0].y, c[0].z);
		glVertex3f(v[0].x, v[0].y, v[0].z);

		glColor3f(c[1].x, c[1].y, c[1].z);
		glVertex3f(v[1].x, v[1].y, v[1].z);

		glColor3f(c[2].x, c[2].y, c[2].z);
		glVertex3f(v[2].x, v[2].y, v[2].z);
	glEnd();
}

// Render the triangle on CPU
void Triangle::RenderCPU() 
{
}
#pragma once

#include <stdlib.h>
#include <math.h>

#include <glm/glm.hpp>


class Triangle {
	private:
		glm::vec3 v[3];		// Triangle vertices
		glm::vec3 c[3];		// Vertex color

	public:
		// Constructor
		Triangle(glm::vec3 &v0, glm::vec3 &v1, glm::vec3 &v2);

		// Rendering the triangle using OpenGL
		void RenderOpenGL(glm::mat4 &modelViewMatrix, glm::mat4 &projectionMatrix);

		// Rendering the triangle using CPU
		void RenderCPU();
};

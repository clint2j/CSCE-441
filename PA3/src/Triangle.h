#pragma once

#include <array>
#include <math.h>
#include <optional>
#include <stdlib.h>
#include <vector>

#include <glm/glm.hpp>

class Triangle
{
  private:
    glm::vec3 v[3]; // Triangle vertices
    glm::vec3 c[3]; // Vertex color
    std::vector<std::array<glm::vec4, 3>> getClippedTriangles(std::array<glm::vec4, 3> startingTriangle,
                                                              int width, int height);

    void rasterize(std::array<glm::vec3, 3> tri, std::array<glm::vec3, 3> triColors, float *color,
                   float *depth, int height, int width);

    std::optional<std::array<glm::vec3, 3>> getColor(std::array<glm::vec3, 3> tri,
                                                     std::array<glm::vec3, 3> triColors, int x, int y);

  public:
    // Constructor
    Triangle(glm::vec3 &v0, glm::vec3 &v1, glm::vec3 &v2);

    // Rendering the triangle using OpenGL
    void RenderOpenGL(glm::mat4 &modelViewMatrix, glm::mat4 &projectionMatrix);

    // Rendering the triangle using CPU
    void RenderCPU(glm::mat4 &modelViewMatrix, glm::mat4 &projectionMatrix, float *color, float *depth,
                   int windowHeight, int windowWidth);

    void ChangeColor(std::array<glm::vec3, 3> arr);

    void ChangeColorByZ(float min, float max);

    float getMaxZ()
    {
        float max = -1e9;
        for (int i = 0; i < 3; ++i) {
            if (v[i].z > max) {
                max = v[i].z;
            }
        }
        return max;
    }
    float getMinZ()
    {
        float min = 1e9;
        for (int i = 0; i < 3; ++i) {
            if (v[i].z < min) {
                min = v[i].z;
            }
        }
        return min;
    }
};

#include "Triangle.h"
#include <GL/glew.h>
#include <algorithm>
#include <glm/gtc/type_ptr.hpp>
#include <iomanip>
#include <iostream>

// A function clamping the input values to the lower and higher bounds
#define CLAMP(in, low, high) ((in) < (low) ? (low) : ((in) > (high) ? (high) : in))

std::vector<std::array<glm::vec4, 3>> Triangle::getClippedTriangles(std::array<glm::vec4, 3> startingTriangle,
                                                                    int width, int height)
{
    return {startingTriangle};
}

void Triangle::rasterize(std::array<glm::vec3, 3> tri, std::array<glm::vec3, 3> triColors, float *color,
                         float *depth, int height, int width)
{
}

std::optional<std::array<glm::vec3, 3>> Triangle::getColor(std::array<glm::vec3, 3> tri,
                                                           std::array<glm::vec3, 3> triColors, int x, int y)
{
    return std::optional<std::array<glm::vec3, 3>>();
}

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
void Triangle::RenderCPU(glm::mat4 &modelViewMatrix, glm::mat4 &projectionMatrix, float *color, float *depth,
                         int windowHeight, int windowWidth)
{
    std::array<glm::vec4, 3> clipSpaceVertex;
    glm::mat4 initalTransformation = projectionMatrix * modelViewMatrix;
    for (int i = 0; i < 3; ++i) {
        glm::vec4 currentObjSpace = glm::vec4(v[i], 1.0f);
        clipSpaceVertex[i] = initalTransformation * currentObjSpace;
    }
    // do clipping
    std::vector<std::array<glm::vec4, 3>> clippedTriangles
        = getClippedTriangles(clipSpaceVertex, windowWidth, windowHeight);

    std::vector<std::array<glm::vec3, 3>> screenSpaceTriangles;
    glm::vec4 viewport = {0, 0, windowWidth, windowHeight};
    for (auto const &tri : clippedTriangles) {
        std::array<glm::vec3, 3> screenTri;
        for (int i = 0; i < 3; ++i) {
            glm::vec3 homogenizedPt = {tri[i].x / tri[i].w, tri[i].y / tri[i].w, tri[i].z / tri[i].w};
            screenTri[i] = glm::projectNO(homogenizedPt, glm::mat4(1.0f), glm::mat4(1.0f), viewport);
        }
        screenSpaceTriangles.push_back(screenTri);
    }
    // rasterization time!!!
    // rasterize()
    for (auto v : screenSpaceTriangles) {
        for (auto vv : v) {
            // Round to nearest pixel
            int x = std::round(vv.x);
            int y = std::round(vv.y);

            // Check if the point is within the screen bounds
            if (x >= 0 && x < windowWidth && y >= 0 && y < windowHeight) {
                // Calculate the index in the color buffer
                int index = (y * windowWidth + x) * 3;

                // Set the color (using the values you provided: 0.8, 0.5, 0.8)
                color[index] = 0.8f;     // Red
                color[index + 1] = 0.5f; // Green
                color[index + 2] = 0.8f; // Blue

                // If you want to use the depth buffer, uncomment the following line
                // depth[y * windowWidth + x] = screenSpaceVertex.z;
            }
        }
    }
}

void Triangle::ChangeColor(std::array<glm::vec3, 3> arr)
{
    std::cout << "ChangeColor" << '\n';
    // c[0] = glm::vec3(88.0f / 255.0f, 150.0f / 255.0f, 88.0f / 255.0f);
    // std::cout << std::fixed << std::setprecision(3) << c[0].x << c[0].y << c[0].z << '\n';
    // c[1] = glm::vec3(88.0f / 255.0f, 150.0f / 255.0f, 88.0f / 255.0f);
    // c[2] = glm::vec3(88.0f / 255.0f, 150.0f / 255.0f, 88.0f / 255.0f);
    for (std::size_t i = 0; i < arr.size(); ++i) {
        c[i] = arr[i];
    }
}

void Triangle::ChangeColorByZ(float min, float max)
{
    for (int i = 0; i < 3; ++i) {
        c[i] = {(v[i].z - min) * 1.0f / (max - min), 0, 0};
    }
}

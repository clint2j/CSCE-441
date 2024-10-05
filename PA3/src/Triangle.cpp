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
    float maxHeight = -1e9;
    float minHeight = 1e9;
    float maxWidth = -1e9;
    float minWidth = 1e9;
    for (const auto &v : tri) {
        maxHeight = std::max(maxHeight, v.y);
        maxWidth = std::max(maxWidth, v.x);
        minHeight = std::min(minHeight, v.y);
        minWidth = std::min(minWidth, v.x);
    }
    int startX = std::max(0, static_cast<int>(std::floor(minWidth)));
    int endX = std::min(width - 1, static_cast<int>(std::ceil(maxWidth)));
    int startY = std::max(0, static_cast<int>(std::floor(minHeight)));
    int endY = std::min(height - 1, static_cast<int>(std::ceil(maxHeight)));
    for (int x = startX; x <= endX; ++x) {
        for (int y = startY; y <= endY; ++y) {
            auto pixelInfo = getColor(tri, triColors, x, y);
            if (!pixelInfo) {
                continue;
            }
            auto [colorPixel, trueZ] = pixelInfo.value();
            if (trueZ <= depth[y * width + x]) {
                depth[y * width + x] = trueZ;
                int index = (y * width + x) * 3;
                color[index] = colorPixel.r;
                color[index + 1] = colorPixel.g;
                color[index + 2] = colorPixel.b;
            }
        }
    }
}

std::optional<std::pair<glm::vec3, float>>
Triangle::getColor(std::array<glm::vec3, 3> tri, std::array<glm::vec3, 3> triColors, int x, int y)
{
    const float epsilon = 1e-6f;
    float v = (y - tri[1].y) * (tri[2].x - tri[1].x) - (x - tri[1].x) * (tri[2].y - tri[1].y);
    v /= (tri[0].y - tri[1].y) * (tri[2].x - tri[1].x) - (tri[0].x - tri[1].x) * (tri[2].y - tri[1].y);
    float w = (y - tri[2].y) * (tri[0].x - tri[2].x) - (x - tri[2].x) * (tri[0].y - tri[2].y);
    w /= (tri[1].y - tri[2].y) * (tri[0].x - tri[2].x) - (tri[1].x - tri[2].x) * (tri[0].y - tri[2].y);
    float u = 1.0f - v - w;
    if (u >= -epsilon && v >= -epsilon && w >= -epsilon && u <= 1 + epsilon && v <= 1 + epsilon
        && w <= 1 + epsilon) {
        glm::vec3 color = u * triColors[0] + v * triColors[1] + w * triColors[2];
        float trueZ = u * tri[0].z + v * tri[1].z + w * tri[2].z;
        return std::make_pair(color, trueZ);
    }
    // Point is outside the triangle
    return std::nullopt;
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
    std::vector<std::array<glm::vec3, 3>> screenSpaceTrianglesColor;
    screenSpaceTrianglesColor.push_back(c);
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
    for (std::size_t i = 0; i < screenSpaceTriangles.size(); ++i) {
        rasterize(screenSpaceTriangles[i], screenSpaceTrianglesColor[i], color, depth, windowHeight,
                  windowWidth);
    }
}

void Triangle::ChangeColor(std::array<glm::vec3, 3> arr)
{
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

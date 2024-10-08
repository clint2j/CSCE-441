#define GLEW_STATIC
#include "MatrixStack.h"
#include "Program.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <memory>
#include <optional>
#include <utility>
#include <vector>

#include "Robot.h"
using std::pair;

using mousePos = std::optional<glm::vec2>;

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800

std::string resource_path = "";

GLFWwindow *window;
double currentXpos, currentYpos;
glm::vec3 eye(0.0f, 0.0f, 8.0f);
glm::vec3 center(0.0f, 0.0f, 0.0f);
glm::vec3 up(0.0f, 1.0f, 0.0f);

mousePos lastMousePos = {};

Program program;
MatrixStack modelViewProjectionMatrix;

// Draw cube on screen
void DrawCube(glm::mat4 &modelViewProjectionMatrix)
{
    program.SendUniformData(modelViewProjectionMatrix, "mvp");
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

Robot robot;

void Display()
{
    program.Bind();

    modelViewProjectionMatrix.loadIdentity();
    modelViewProjectionMatrix.pushMatrix();

    // Setting the view and Projection matrices
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    modelViewProjectionMatrix.Perspective(glm::radians(60.0f), float(width) / float(height), 0.1f, 100.0f);
    modelViewProjectionMatrix.LookAt(eye, center, up);

    // // Model transformation for Cube 1
    // modelViewProjectionMatrix.pushMatrix();
    // modelViewProjectionMatrix.translate(-2.0f, 2.0f, 0.0f);
    // modelViewProjectionMatrix.rotateY(glm::radians(45.0f));
    // modelViewProjectionMatrix.scale(0.8);
    // DrawCube(modelViewProjectionMatrix.topMatrix());
    // modelViewProjectionMatrix.popMatrix();

    // // Model transformation for Cube 2
    // modelViewProjectionMatrix.pushMatrix();
    // modelViewProjectionMatrix.translate(-2.0f, -2.0f, 0.0f);
    // modelViewProjectionMatrix.rotateX(glm::radians(45.0f));
    // modelViewProjectionMatrix.scale(0.8);
    // DrawCube(modelViewProjectionMatrix.topMatrix());
    // modelViewProjectionMatrix.popMatrix();

    // // Model transformation for Cube 3
    // modelViewProjectionMatrix.pushMatrix();
    // modelViewProjectionMatrix.translate(2.0f, 0.0f, 0.0f);
    // modelViewProjectionMatrix.rotateZ(glm::radians(45.0f));
    // modelViewProjectionMatrix.scale(0.8);
    // DrawCube(modelViewProjectionMatrix.topMatrix());
    // modelViewProjectionMatrix.popMatrix();

    robot.draw(modelViewProjectionMatrix, program);

    modelViewProjectionMatrix.popMatrix();

    program.Unbind();
}

// Mouse callback function
void MouseCallback(GLFWwindow *lWindow, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        std::cout << "Mouse left button is pressed." << std::endl;
    else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        std::cout << "Mouse Released!!!!\n";
        lastMousePos = {};
    }
}

// Mouse position callback function
void CursorPositionCallback(GLFWwindow *lWindow, double xpos, double ypos)
{
    int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    int state2 = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
    glm::vec3 viewDirection = glm::normalize(center - eye);
    glm::vec3 right = glm::cross(glm::vec3(viewDirection), glm::vec3(up));
    right = glm::normalize(right);
    glm::vec2 currMouse = {xpos, ypos};
    // handle scroll zoom here
    if (!lastMousePos) {
        lastMousePos = currMouse;
        return;
    }
    glm::vec2 diff = currMouse - lastMousePos.value();
    if (state == GLFW_PRESS) {
        if (diff.x != 0) {
            glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(diff.x), up);
            eye = glm::vec3(rotationMatrix * glm::vec4(eye, 1.0f));
        }
        if (diff.y != 0) {
            glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(diff.y), right);
            eye = glm::vec3(rotationMatrix * glm::vec4(eye, 1.0f));
        }
    } else if (state2 == GLFW_PRESS) {
        if (diff.x != 0) {
            auto translateMatrix = glm::translate(glm::mat4(1.0f), right * (float)(diff.y * .6));
            eye = glm::vec3(translateMatrix * glm::vec4(eye, 1.0f));
            center = glm::vec3(translateMatrix * glm::vec4(center, 1.0f));
        }
        if (diff.y != 0) {
            glm::vec3 up = glm::normalize(glm::cross(viewDirection, right));
            auto translateMatrix = glm::translate(glm::mat4(1.0f), up * (float)(diff.y * .6));
            eye = glm::vec3(translateMatrix * glm::vec4(eye, 1.0f));
            center = glm::vec3(translateMatrix * glm::vec4(center, 1.0f));
        }
    }
    lastMousePos = currMouse;
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    std::cout << "scrolled: (" << xoffset << ", " << yoffset << ")\n";

    // Calculate the current view direction
    glm::vec3 viewDirection = glm::normalize(eye - center);

    // Modify the eye position based on yoffset
    eye -= viewDirection * static_cast<float>(yoffset) * 0.5f;
}

// Keyboard character callback function
void CharacterCallback(GLFWwindow *lWindow, unsigned int key)
{
    std::cout << "Key " << (char)key << " is pressed." << std::endl;
    switch (key) {
        case '.':
            robot.next();
            break;
        case ',':
            robot.prev();
            break;
        case 'x':
            robot.rotateX(glm::radians(-15.0f));
            break;
        case 'X':
            robot.rotateX(glm::radians(15.0f));
            break;
        case 'y':
            robot.rotateY(glm::radians(-15.0f));
            break;
        case 'z':
            robot.rotateZ(glm::radians(-15.0f));
            break;
        case 'Z':
            robot.rotateZ(glm::radians(15.0f));
            break;
        case 'Y':
            robot.rotateY(glm::radians(15.0f));
            break;
    }
}

void CreateCube()
{
    // x, y, z, r, g, b, ...
    float cubeVerts[] = {// Face x-
                         -1.0f, +1.0f, +1.0f, 0.8f, 0.2f, 0.2f, -1.0f, +1.0f, -1.0f, 0.8f, 0.2f, 0.2f, -1.0f,
                         -1.0f, +1.0f, 0.8f, 0.2f, 0.2f, -1.0f, -1.0f, +1.0f, 0.8f, 0.2f, 0.2f, -1.0f, +1.0f,
                         -1.0f, 0.8f, 0.2f, 0.2f, -1.0f, -1.0f, -1.0f, 0.8f, 0.2f, 0.2f,
                         // Face x+
                         +1.0f, +1.0f, +1.0f, 0.8f, 0.2f, 0.2f, +1.0f, -1.0f, +1.0f, 0.8f, 0.2f, 0.2f, +1.0f,
                         +1.0f, -1.0f, 0.8f, 0.2f, 0.2f, +1.0f, +1.0f, -1.0f, 0.8f, 0.2f, 0.2f, +1.0f, -1.0f,
                         +1.0f, 0.8f, 0.2f, 0.2f, +1.0f, -1.0f, -1.0f, 0.8f, 0.2f, 0.2f,
                         // Face y-
                         +1.0f, -1.0f, +1.0f, 0.2f, 0.8f, 0.2f, -1.0f, -1.0f, +1.0f, 0.2f, 0.8f, 0.2f, +1.0f,
                         -1.0f, -1.0f, 0.2f, 0.8f, 0.2f, +1.0f, -1.0f, -1.0f, 0.2f, 0.8f, 0.2f, -1.0f, -1.0f,
                         +1.0f, 0.2f, 0.8f, 0.2f, -1.0f, -1.0f, -1.0f, 0.2f, 0.8f, 0.2f,
                         // Face y+
                         +1.0f, +1.0f, +1.0f, 0.2f, 0.8f, 0.2f, +1.0f, +1.0f, -1.0f, 0.2f, 0.8f, 0.2f, -1.0f,
                         +1.0f, +1.0f, 0.2f, 0.8f, 0.2f, -1.0f, +1.0f, +1.0f, 0.2f, 0.8f, 0.2f, +1.0f, +1.0f,
                         -1.0f, 0.2f, 0.8f, 0.2f, -1.0f, +1.0f, -1.0f, 0.2f, 0.8f, 0.2f,
                         // Face z-
                         +1.0f, +1.0f, -1.0f, 0.2f, 0.2f, 0.8f, +1.0f, -1.0f, -1.0f, 0.2f, 0.2f, 0.8f, -1.0f,
                         +1.0f, -1.0f, 0.2f, 0.2f, 0.8f, -1.0f, +1.0f, -1.0f, 0.2f, 0.2f, 0.8f, +1.0f, -1.0f,
                         -1.0f, 0.2f, 0.2f, 0.8f, -1.0f, -1.0f, -1.0f, 0.2f, 0.2f, 0.8f,
                         // Face z+
                         +1.0f, +1.0f, +1.0f, 0.2f, 0.2f, 0.8f, -1.0f, +1.0f, +1.0f, 0.2f, 0.2f, 0.8f, +1.0f,
                         -1.0f, +1.0f, 0.2f, 0.2f, 0.8f, +1.0f, -1.0f, +1.0f, 0.2f, 0.2f, 0.8f, -1.0f, +1.0f,
                         +1.0f, 0.2f, 0.2f, 0.8f, -1.0f, -1.0f, +1.0f, 0.2f, 0.2f, 0.8f};

    GLuint vertBufferID;
    glGenBuffers(1, &vertBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, vertBufferID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVerts), cubeVerts, GL_STATIC_DRAW);
    GLint posID = glGetAttribLocation(program.GetPID(), "position");
    glEnableVertexAttribArray(posID);
    glVertexAttribPointer(posID, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
    GLint colID = glGetAttribLocation(program.GetPID(), "color");
    glEnableVertexAttribArray(colID);
    glVertexAttribPointer(colID, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
}

void FrameBufferSizeCallback(GLFWwindow *lWindow, int width, int height) { glViewport(0, 0, width, height); }

void Init()
{
    glfwInit();
    glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GL_FALSE);
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Assignment2 - Clint Hart", NULL, NULL);
    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    glewInit();
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glfwSetMouseButtonCallback(window, MouseCallback);
    glfwSetCursorPosCallback(window, CursorPositionCallback);
    glfwSetCharCallback(window, CharacterCallback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetFramebufferSizeCallback(window, FrameBufferSizeCallback);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    program.SetShadersFileName(resource_path + "shader.vert", resource_path + "shader.frag");
    program.Init();

    robot = {};

    CreateCube();
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        std::cout << "Need to specify the resource path as the input argument." << std::endl;
        return 0;
    } else
        resource_path = argv[1];

    Init();
    while (glfwWindowShouldClose(window) == 0) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        Display();
        glFlush();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
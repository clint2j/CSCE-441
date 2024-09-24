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
class bodyPart
{
  private:
    glm::vec3 tp, sp, rc, tc, originalSp;
    std::shared_ptr<bodyPart> child;

  public:
    bodyPart(glm::vec3 _tp, glm::vec3 _sp, glm::vec3 _tc, std::shared_ptr<bodyPart> _child = nullptr)
        : tp(_tp), sp(_sp), rc(), tc(_tc), originalSp(_sp), child(_child)
    {
    }
    void draw()
    {
        modelViewProjectionMatrix.pushMatrix();
        modelViewProjectionMatrix.translate(tc);
        modelViewProjectionMatrix.rotateX(rc.x);
        modelViewProjectionMatrix.rotateY(rc.y);
        modelViewProjectionMatrix.rotateZ(rc.z);
        modelViewProjectionMatrix.pushMatrix();
        modelViewProjectionMatrix.translate(tp);
        modelViewProjectionMatrix.scale(sp);
        DrawCube(modelViewProjectionMatrix.topMatrix());
        modelViewProjectionMatrix.popMatrix();
        if (child) {
            child->draw();
        }
        modelViewProjectionMatrix.popMatrix();
    }
    void rotateZ(float z_rotate) { rc.z += z_rotate; }
    void rotateY(float y_rotate) { rc.y += y_rotate; }
    void rotateX(float x_rotate) { rc.x += x_rotate; }
    void select()
    {
        sp.x *= 1.1;
        sp.y *= 1.1;
        sp.z *= 1.1;
    }
    void unSelect()
    {
        sp.x /= 1.1;
        sp.y /= 1.1;
        sp.z /= 1.1;
    }
    std::shared_ptr<bodyPart> next() { return child; }
};
// Robot elements holds torso
class RobotElements
{
  private:
    std::array<std::shared_ptr<bodyPart>, 5> bodyParts;
    glm::vec3 tp, sp, rc, tc, originalSp;
    int indexOfTree;
    bool isChild;
    std::shared_ptr<bodyPart> currentChild;

  public:
    RobotElements() : indexOfTree(-1), isChild(false)
    {
        tp = {0, 0, 0};
        sp = {.8, 1.4, .5};
        rc = {0, 0, 0};
        tc = {0, 1.1, 0};
        originalSp = sp;
        // head (tp, sp, /rc/, tc)
        bodyParts[0] = std::make_shared<bodyPart>(glm::vec3(0, .3, 0), glm::vec3(.33, .33, .33),
                                                  glm::vec3(0, 1.4, 0), nullptr);
        // arms
        std::shared_ptr<bodyPart> lowerLeftArm = std::make_shared<bodyPart>(
            glm::vec3(-1.10, 0, 0), glm::vec3(.5, .25, .25), glm::vec3(-.8, 0, 0), nullptr);
        bodyParts[1] = std::make_shared<bodyPart>(glm::vec3(-.7, 0, 0), glm::vec3(.7, .3, .3),
                                                  glm::vec3(-.8, .5, 0), lowerLeftArm);
        std::shared_ptr<bodyPart> lowerRightArm = std::make_shared<bodyPart>(
            glm::vec3(1.10, 0, 0), glm::vec3(.5, .25, .25), glm::vec3(.8, 0, 0), nullptr);
        bodyParts[2] = std::make_shared<bodyPart>(glm::vec3(.7, 0, 0), glm::vec3(.7, .3, .3),
                                                  glm::vec3(.8, .5, 0), lowerRightArm);
        // legs
        std::shared_ptr<bodyPart> lowerLeftLeg = std::make_shared<bodyPart>(
            glm::vec3(0, 0, 0), glm::vec3(.27, .7, .27), glm::vec3(0, -1.4, 0), nullptr);
        bodyParts[3] = std::make_shared<bodyPart>(glm::vec3(0, 0, 0), glm::vec3(.32, .7, .32),
                                                  glm::vec3(-.48, -2, 0), lowerLeftLeg);
        std::shared_ptr<bodyPart> lowerRightLeg = std::make_shared<bodyPart>(
            glm::vec3(0, 0, 0), glm::vec3(.27, .7, .27), glm::vec3(0, -1.4, 0), nullptr);
        bodyParts[4] = std::make_shared<bodyPart>(glm::vec3(0, 0, 0), glm::vec3(.32, .7, .32),
                                                  glm::vec3(.48, -2, 0), lowerRightLeg);
        currentChild = nullptr;
    }
    void draw()
    {
        modelViewProjectionMatrix.pushMatrix();
        modelViewProjectionMatrix.translate(tc);
        modelViewProjectionMatrix.rotateX(rc.x);
        modelViewProjectionMatrix.rotateY(rc.y);
        modelViewProjectionMatrix.rotateZ(rc.z);
        modelViewProjectionMatrix.pushMatrix();
        modelViewProjectionMatrix.translate(tp);
        modelViewProjectionMatrix.scale(sp);
        DrawCube(modelViewProjectionMatrix.topMatrix());
        modelViewProjectionMatrix.popMatrix();
        for (auto p : bodyParts) {
            p->draw();
        }
        modelViewProjectionMatrix.popMatrix();
    }
    void rotateZ(float z_rotate)
    {
        if (indexOfTree == 0) {
            rc.z += z_rotate;
        } else if (indexOfTree < 6 && indexOfTree >= 1) {
        }
    }
    void rotateY(float y_rotate) { rc.y += y_rotate; }
    void rotateX(float x_rotate) { rc.x += x_rotate; }
    void select()
    {
        sp.x *= 1.1;
        sp.y *= 1.1;
        sp.z *= 1.1;
    }
    void unSelect()
    {
        sp.x /= 1.1;
        sp.y /= 1.1;
        sp.z /= 1.1;
    }
    void next()
    {
        if (indexOfTree != 6) {
            if (isChild) {
                currentChild->unSelect();
            } else if (indexOfTree == 0) {
                unSelect();
            } else if (indexOfTree != -1) {
                bodyParts[indexOfTree]->unSelect();
            }
            if (indexOfTree >= 1 && (!isChild && bodyParts[indexOfTree - 1]->next())) {
                isChild = true;
                currentChild = bodyParts[indexOfTree - 1]->next();
                currentChild->select();
            }
            ++indexOfTree;
            isChild = false;
        }
    }
    void back()
    {
        if (indexOfTree != -1) {
            --indexOfTree;
        }
    }
    // std::shared_ptr<bodyPart> next() { return child; }
};

RobotElements robot;

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

    robot.draw();

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
    glm::vec4 viewDirection = glm::vec4(glm::normalize(center - eye), 0.0f);
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
        // if (diff.x != 0) {
        //     glm::mat4 movementMatrix = glm::translate(glm::mat4(1.0f), (glm::vec3(diff.x * .06, 0, 0)));
        //     eye = glm::vec3(movementMatrix * glm::vec4(eye, 1.0f));
        //     center = glm::vec3(movementMatrix * glm::vec4(center, 1.0f));
        // } else if (diff.y != 0) {
        //     glm::mat4 movementMatrix = glm::translate(glm::mat4(1.0f), (glm::vec3(0, diff.y * .06, 0)));
        //     eye = glm::vec3(movementMatrix * glm::vec4(eye, 1.0f));
        //     center = glm::vec3(movementMatrix * glm::vec4(center, 1.0f));
        // }
    }
    lastMousePos = currMouse;
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    std::cout << "scrolled: (" << xoffset << ", " << yoffset << ")\n";
    // glm::vec4 viewDirection = glm::vec4(glm::normalize(eye - center), 0.0f);
    // glm::mat4 scaleMatrix = glm::scale(
    //     glm::mat4(1.0f), {1 - (yoffset * .02), 1 - (yoffset * .02), 1 -
    //     (yoffset * .02)});lowerRightLeg
    // viewDirection = scaleMatrix * viewDirection;
    // // viewDirection /= viewDirection.w;
    // // glm::vec3 newEye = center + glm::vec3(viewDirection);
    // std::cout << "old eye: " << eye.x << eye.y << eye.z << '\n';
    // eye = center + glm::vec3(viewDirection);
    // std::cout << "new eye: " << eye.x << eye.y << eye.z << '\n';
    // std::cout << "center: " << center.x << center.y << center.z << '\n';
}

// Keyboard character callback function
void CharacterCallback(GLFWwindow *lWindow, unsigned int key)
{
    std::cout << "Key " << (char)key << " is pressed." << std::endl;
    if (key == 'f') {
        robot.rotateY(glm::radians(45.0f));
    } else if (key == 'g') {
        robot.select();
    } else if (key == '.') {
        robot.next();
    } else if (key == 'm') {
        robot.unSelect();
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
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

class RobotNode
{
  public:
    glm::vec3 tp, sp, rc, tc;
    std::weak_ptr<RobotNode> parent;
    std::vector<std::shared_ptr<RobotNode>> children;
    RobotNode(glm::vec3 _tp, glm::vec3 _sp, glm::vec3 _tc) : tp(_tp), sp(_sp), rc(), tc(_tc), children() {}
    void draw(MatrixStack &modelViewProjectionMatrix, Program &program);
};

class Robot
{
  public:
    std::shared_ptr<RobotNode> root, current;
    Robot();
    void draw(MatrixStack &stack, Program &program);
    void next();
    void prev();
};
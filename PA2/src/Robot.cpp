#include "Robot.h"

// RobotNode(glm::vec3 _tp, glm::vec3 _sp, glm::vec3 _tc)

Robot::Robot() : current(nullptr)
{
    root = std::make_shared<RobotNode>(glm::vec3(0, 0, 0), glm::vec3(.8, 1.4, .5), glm::vec3(0, 1.1, 0));
    root->children.emplace_back(
        std::make_shared<RobotNode>(glm::vec3(0, .3, 0), glm::vec3(.33, .33, .33), glm::vec3(0, 1.4, 0)));
    root->children.emplace_back(
        std::make_shared<RobotNode>(glm::vec3(-.7, 0, 0), glm::vec3(.7, .3, .3), glm::vec3(-.8, .5, 0)));
    root->children.emplace_back(
        std::make_shared<RobotNode>(glm::vec3(.7, 0, 0), glm::vec3(.7, .3, .3), glm::vec3(.8, .5, 0)));
    root->children.emplace_back(
        std::make_shared<RobotNode>(glm::vec3(0, 0, 0), glm::vec3(.32, .7, .32), glm::vec3(-.48, -2, 0)));
    root->children.emplace_back(
        std::make_shared<RobotNode>(glm::vec3(0, 0, 0), glm::vec3(.32, .7, .32), glm::vec3(.48, -2, 0)));
    root->children[1]->children.emplace_back(
        std::make_shared<RobotNode>(glm::vec3(-1.10, 0, 0), glm::vec3(.5, .25, .25), glm::vec3(-.8, 0, 0)));
    root->children[2]->children.emplace_back(
        std::make_shared<RobotNode>(glm::vec3(1.10, 0, 0), glm::vec3(.5, .25, .25), glm::vec3(.8, 0, 0)));
    root->children[3]->children.emplace_back(
        std::make_shared<RobotNode>(glm::vec3(0, 0, 0), glm::vec3(.27, .7, .27), glm::vec3(0, -1.4, 0)));
    root->children[4]->children.emplace_back(
        std::make_shared<RobotNode>(glm::vec3(0, 0, 0), glm::vec3(.27, .7, .27), glm::vec3(0, -1.4, 0)));
}

void RobotNode::draw(MatrixStack &modelViewProjectionMatrix, Program &program)
{
    modelViewProjectionMatrix.pushMatrix();
    modelViewProjectionMatrix.translate(tc);
    modelViewProjectionMatrix.rotateX(rc.x);
    modelViewProjectionMatrix.rotateY(rc.y);
    modelViewProjectionMatrix.rotateZ(rc.z);
    modelViewProjectionMatrix.pushMatrix();
    modelViewProjectionMatrix.translate(tp);
    modelViewProjectionMatrix.scale(sp);
    program.SendUniformData(modelViewProjectionMatrix.topMatrix(), "mvp");
    glDrawArrays(GL_TRIANGLES, 0, 36);
    modelViewProjectionMatrix.popMatrix();
    for (auto child : children) {
        child->draw(modelViewProjectionMatrix, program);
    }
    modelViewProjectionMatrix.popMatrix();
}

void Robot::draw(MatrixStack &stack, Program &program) { root->draw(stack, program); }

void Robot::next()
{
    if (current && current->children.size() > 0) {
        current = current->children[0];
    }
}

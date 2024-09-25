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

void RobotNode::rotateZ(float z_rotate) { rc.z += z_rotate; }

void RobotNode::rotateY(float y_rotate) { rc.y += y_rotate; }

void RobotNode::rotateX(float x_rotate) { rc.x += x_rotate; }

void RobotNode::select()
{
    sp.x *= 1.1;
    sp.y *= 1.1;
    sp.z *= 1.1;
}

void RobotNode::unSelect()
{
    sp.x /= 1.1;
    sp.y /= 1.1;
    sp.z /= 1.1;
}

void Robot::draw(MatrixStack &stack, Program &program) { root->draw(stack, program); }

void Robot::next()
{
    if (current) {
        current->unSelect();
    } else {
        current = root;
        current->select();
        return;
    }
    if (level == 0) {
        level = 1;
        num = 0;
        current = current->children[num++];
    } else if (level == 1) {
        if (current->children.size() > 0) {
            level = 2;
            current = current->children[0];
        } else {
            if (num < root->children.size()) {
                current = root->children[num++];
            } else {
                level = 0;
                current = nullptr;
            }
        }
    } else {
        level = 1;
        if (num < root->children.size()) {
            current = root->children[num++];
        } else {
            level = 0;
            current = nullptr;
        }
    }
    if (current) {
        current->select();
    }
}

void Robot::prev()
{
    if (current) {
        current->unSelect();
    } else {
        num = 4;
        level = 2;
        current = root->children[num]->children[0];
        current->select();
        return;
    }
    if (level == 0) {
        current = nullptr;
    } else if (level == 1) {
        if (num == 0) {
            current = root;
            level = 0;
        } else if (num == 1) {
            current = root->children[0];
            num = 0;
        } else {
            level = 2;
            --num;
            current = root->children[num]->children[0];
        }
    } else {
        level = 1;
        current = root->children[num];
    }
    if (current) {
        current->select();
    }
}

void Robot::rotateZ(float z_rotate)
{
    if (current) {
        current->rotateZ(z_rotate);
    }
}

void Robot::rotateY(float y_rotate)
{
    if (current) {
        current->rotateY(y_rotate);
    }
}

void Robot::rotateX(float x_rotate)
{
    if (current) {
        current->rotateX(x_rotate);
    }
}

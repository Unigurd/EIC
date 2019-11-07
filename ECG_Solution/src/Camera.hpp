#pragma once

#include "glm\matrix.hpp"
#include <glm/gtc/type_ptr.hpp>
#include "glm/ext.hpp"

// The Camera class
class Camera {
private:
    glm::mat4 projection; // This is constant
    glm::mat4 translation;
    glm::mat4 rotation;
    float rotationX, rotationY; // 1.0 is one full rotation
    glm::mat4 viewProj;
    void updateViewProj();

public:
    Camera(float fov, float height, float width, float zNear, float zFar);
    Camera(float fov, float height, float width, float zNear, float zFar, glm::vec3 trans, glm::vec3 rot);
    glm::mat4 ViewProjMatrix();
    void translate(glm::vec3 trans);
    void rotate(glm::vec3 rot);
};

#pragma once

#include "glm\matrix.hpp"
#include <glm/gtc/type_ptr.hpp>
#include "glm/ext.hpp"
#include <GL\glew.h>
#include <GLFW/glfw3.h>

// The Camera class
class Camera {
private:
    glm::mat4 projection; // This is constant
    glm::mat4 translation;
    glm::mat4 rotation;
    float rotationX, rotationY; // 1.0 is one full rotation
    glm::mat4 viewProj;
    void updateViewProj();
    bool wireframe;
    bool backfaceCulling;

public:
    Camera(float fov, int height, int width, float zNear, float zFar);
    glm::mat4 ViewProjMatrix();
    void translate(glm::vec3 trans);
    void rotate(glm::vec3 rot);
    void toggleBackfaceCulling();
    void toggleWireframe();
};

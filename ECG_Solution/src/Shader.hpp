#pragma once

#include <string>
#include <GL\glew.h>
#include <GLFW/glfw3.h>
#include "glm\matrix.hpp"
#include "glm/ext.hpp"
#include "Camera.hpp"
#include "Shapes\Shape.hpp"

class Shader {
private:
    unsigned int shaderID;
    int viewProjLocation;
    int viewPosLocation;
public:
    // TODO move rest of shape-related parameters into the Shape
    Shader(std::string vertexShaderString, std::string fragmentShaderString, glm::vec3 pos, glm::vec3 rot, glm::vec3 sca, glm::vec3 col, Shape shape);
    unsigned int ID();
    int ViewProjLocation();
    int ViewPosLocation();
};

// This struct exists to make sure that the previously bound
// shader is restored when this binding goes out of scope
struct BindShader {
private:
    GLint prevId;

public:
    BindShader(Shader &shader, Camera &camera);
    BindShader(Shader &shader, glm::mat4 viewProjMatrix);
    ~BindShader();
};

#pragma once

#include <string>
#include <GL\glew.h>
#include <GLFW/glfw3.h>
#include "glm\matrix.hpp"
#include "glm/ext.hpp"

class Shader {
private:
    unsigned int shaderID;
    int viewProjLocation;
public:
    Shader(std::string vertexShaderString, std::string fragmentShaderString, glm::vec3 pos, glm::vec3 rot, glm::vec3 sca, glm::vec3 col);
    unsigned int ID();
    int ViewProjLocation();
};

// This struct exists to make sure that the previously bound
// shader is restored when this binding goes out of scope
struct BindShader {
private:
    GLint prevId;

public:
    BindShader(Shader &shader, glm::mat4 viewProjMatrix);
    ~BindShader();
};

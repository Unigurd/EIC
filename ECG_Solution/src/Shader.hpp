#pragma once

#include <string>
#include <GL\glew.h>
#include <GLFW/glfw3.h>
#include "glm\matrix.hpp"
#include "glm/ext.hpp"
#include "Camera.hpp"
#include "Shapes\Shape.hpp"
#include "Lights.hpp"

class Shader {
private:
    unsigned int shaderID;
    int viewProjLocation;
    int cameraPosLocation;
public:
    // TODO move rest of shape-related parameters into the Shape
    Shader(std::string vertexShaderString, std::string fragmentShaderString, Shape shape, Lights lights);
    unsigned int ID();
    int ViewProjLocation();
    int CameraPosLocation();
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

#pragma once
#include <GL\glew.h>
#include <GLFW/glfw3.h>
#include "glm\matrix.hpp"
#include "glm/ext.hpp"

struct PointLight {
    glm::vec3 color;
    glm::vec3 position;
    glm::vec3 attenuation;
};

struct DirectionLight {
    glm::vec3 color;
    glm::vec3 direction;
};

struct Lights {
    PointLight pointLight;
    DirectionLight dirLight;
};

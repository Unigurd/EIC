#pragma once

#include "glm\matrix.hpp"
#include "glm/ext.hpp"
#include <vector>


// Describes the lighting properties of a surface
struct Surface {
    float ka;
    float kd;
    float ks;
    int alpha;
};

// Describes a transformation into world space
struct Transformation {
    glm::vec3 translation;
    glm::vec3 rotation;
    glm::vec3 scaling;
};

class Shape
{
protected:
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    unsigned int VAO;
    void initVAO();
    Surface surface;
    glm::vec3 color;
    Transformation transformation;

public:
    void Draw();
    Surface &GetSurface();
    glm::vec3 Color();
    Transformation &GetTransformation();
};

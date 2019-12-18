#pragma once

#include "Shape.hpp"

class Cylinder : public Shape {
public:
    Cylinder(float height, float radius, unsigned int sides, Surface srfc, Transformation trans, glm::vec3 col);
    ~Cylinder();
};


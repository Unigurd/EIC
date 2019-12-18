#pragma once

#include "Shape.hpp"

class Box : public Shape {
public:
    Box(float width, float height, float depth, Surface srfc, Transformation trans, glm::vec3 col);
    ~Box();
};


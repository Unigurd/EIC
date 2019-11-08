#pragma once

#include <vector>
#include "Shader.hpp"
#include "Shape.hpp"

class Box : public Shape {
public:
    Box(float width, float height, float depth);
    ~Box();
};


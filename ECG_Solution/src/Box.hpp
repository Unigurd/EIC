#pragma once

#include <vector>
#include "Shader.hpp"

class Box
{
private:
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    unsigned int VAO;

public:
    Box(float width, float height, float depth);
    ~Box();
    void Draw();
};


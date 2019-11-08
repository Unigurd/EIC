#pragma once

#include <vector>

class Shape
{
protected:
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    unsigned int VAO;

public:
    //virtual Shape(float width, float height, float depth) = 0;
    //virtual ~Shape() = 0;
    void Draw();
};

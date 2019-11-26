#pragma once

#include <vector>

class Shape
{
protected:
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    unsigned int VAO;
    void initVAO();

public:
    void Draw();
};
